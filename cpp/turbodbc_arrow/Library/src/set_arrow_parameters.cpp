#include <turbodbc_arrow/set_arrow_parameters.h>

#include <arrow/python/pyarrow.h>

#include <turbodbc/errors.h>
#include <turbodbc/make_description.h>
#include <turbodbc/time_helpers.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sql.h>

#include <ciso646>

#include <boost/locale.hpp>

using arrow::BooleanArray;
using arrow::BinaryArray;
using arrow::ChunkedArray;
using arrow::Date32Array;
using arrow::DoubleType;
using arrow::Int8Type;
using arrow::Int16Type;
using arrow::Int32Type;
using arrow::Int64Type;
using arrow::UInt8Type;
using arrow::UInt16Type;
using arrow::UInt32Type;
using arrow::UInt64Type;
using arrow::NumericArray;
using arrow::StringArray;
using arrow::Table;
using arrow::TimestampArray;
using arrow::TimestampType;
using arrow::TimeUnit;
using arrow::TypeTraits;

namespace turbodbc_arrow {

namespace {

    struct parameter_converter {
        parameter_converter(std::shared_ptr<ChunkedArray> const & data,
                            turbodbc::bound_parameter_set & parameters,
                            std::size_t parameter_index) :
            data(data),
            parameters(parameters),
            parameter_index(parameter_index)
        {}

        cpp_odbc::multi_value_buffer & get_buffer() {
            return parameters.get_parameters()[parameter_index]->get_buffer();
        }

        template <size_t element_size>
        void set_indicator(cpp_odbc::multi_value_buffer & buffer, int64_t start, int64_t elements) {
            // Currently only non-chunked columns are supported
            arrow::Array const& chunk = *data->chunk(0);
            if (chunk.null_count() == 0) {
              std::fill_n(buffer.indicator_pointer(), elements, element_size);
            } else if (chunk.null_count() == chunk.length()) {
              std::fill_n(buffer.indicator_pointer(), elements, SQL_NULL_DATA);
            } else {
              auto const indicator = buffer.indicator_pointer();
              for (int64_t i = 0; i != elements; ++i) {
                indicator[i] = chunk.IsNull(start + i) ? SQL_NULL_DATA : element_size;
              }
            }
        }

        virtual void set_batch(int64_t start, int64_t elements) = 0;

        virtual ~parameter_converter() = default;

        std::shared_ptr<ChunkedArray> data;
        turbodbc::bound_parameter_set & parameters;
        std::size_t const parameter_index;
    };

    struct null_converter : public parameter_converter {
      using parameter_converter::parameter_converter;

      void set_batch(int64_t, int64_t elements) final {
        auto & buffer = get_buffer();
        std::fill_n(buffer.indicator_pointer(), elements, SQL_NULL_DATA);
      }
    };

    struct string_converter : public parameter_converter {
      string_converter(std::shared_ptr<ChunkedArray> const & data,
          turbodbc::bound_parameter_set & parameters,
          std::size_t parameter_index) :
        parameter_converter(data, parameters, parameter_index),
        type(parameters.get_initial_parameter_types()[parameter_index])
      {}

      void rebind_to_maximum_length(BinaryArray const & array, std::size_t start, std::size_t elements)
      {
          int32_t maximum_length = 0;
          for (int64_t i = 0; i != elements; ++i) {
            if (!array.IsNull(start + i)) {
              maximum_length = std::max(maximum_length, array.value_length(start + i));
            }
          }

          // Propagate the maximum string length to the parameters.
          // These then adjust the size of the underlying buffer.
          parameters.rebind(parameter_index, turbodbc::make_description(type, maximum_length));
      }

      template <typename String>
        void set_batch_of_type(std::size_t start, std::size_t elements)
        {
          // Currently only non-chunked columns are supported
          auto const& typed_array = static_cast<const BinaryArray&>(*data->chunk(0));
          rebind_to_maximum_length(typed_array, start, elements);
          auto & buffer = get_buffer();
          auto const character_size = sizeof(typename String::value_type);

          for (int64_t i = 0; i != elements; ++i) {
            auto element = buffer[i];
            if (typed_array.IsNull(start + i)) {
              element.indicator = SQL_NULL_DATA;
            } else {
              int32_t out_length;
              uint8_t const *value = typed_array.GetValue(start + i, &out_length);
              std::memcpy(element.data_pointer, value, out_length);
              element.indicator = character_size * out_length;
            }
          }
        }

      // Parse UTF-16 data and convert it on-the-fly to UTF-8
      void set_batch_utf16(std::size_t start, std::size_t elements)
      {
        auto const& typed_array = static_cast<const BinaryArray&>(*data->chunk(0));
        std::vector<std::pair<bool, std::u16string>> batch;
        size_t maximum_length = 0;
        for (int64_t i = 0; i != elements; ++i) {
          if (!typed_array.IsNull(start + i)) {
            std::u16string str = boost::locale::conv::utf_to_utf<char16_t>(typed_array.GetString(start + i));
            maximum_length = std::max(maximum_length, str.length());
            batch.push_back({false, str});
          } else {
            batch.push_back({true, std::u16string()});
          }
        }

        // Propagate the maximum string length to the parameters.
        // These then adjust the size of the underlying buffer.
        parameters.rebind(parameter_index, turbodbc::make_description(type, maximum_length));
        auto & buffer = get_buffer();
        auto const character_size = sizeof(typename std::u16string::value_type);

        for (std::size_t i = 0; i != batch.size(); ++i) {
            auto element = buffer[i];
            if (batch[i].first) {
                element.indicator = SQL_NULL_DATA;
            } else {
                auto const & s = batch[i].second;
                std::memcpy(element.data_pointer, s.c_str(), character_size * (s.size() + 1));
                element.indicator = character_size * s.size();
            }
        }
      }

      void set_batch(int64_t start, int64_t elements) final
      {
        if (type == turbodbc::type_code::unicode) {
          set_batch_utf16(start, elements);
        } else {
          set_batch_of_type<std::string>(start, elements);
        }
      }

      private:
      turbodbc::type_code type;
    };

    template <typename ArrowType>
    struct numeric_converter : public parameter_converter {
        numeric_converter(std::shared_ptr<ChunkedArray> const & data,
                         turbodbc::bound_parameter_set & parameters,
                         std::size_t parameter_index,
                         turbodbc::type_code type) :
            parameter_converter(data, parameters, parameter_index)
        {
          parameters.rebind(parameter_index, turbodbc::make_description(type, 0));
        }

      void set_batch(int64_t start, int64_t elements) final {
        auto & buffer = get_buffer();

        // Currently only non-chunked columns are supported
        auto const& typed_array = static_cast<const typename TypeTraits<ArrowType>::ArrayType&>(*data->chunk(0));
        typename ArrowType::c_type const* data_ptr = typed_array.raw_values();
        memcpy(buffer.data_pointer(), data_ptr + start, elements * sizeof(typename ArrowType::c_type));

        set_indicator<sizeof(typename ArrowType::c_type)>(buffer, start, elements);
      }
    };

    template <typename SrcArrowType, typename DestArrowType>
    struct casting_numeric_converter : public parameter_converter {
        casting_numeric_converter(std::shared_ptr<ChunkedArray> const & data,
                                 turbodbc::bound_parameter_set & parameters,
                                 std::size_t parameter_index,
                                 turbodbc::type_code type) :
            parameter_converter(data, parameters, parameter_index)
        {
          parameters.rebind(parameter_index, turbodbc::make_description(type, 0));
        }

      void set_batch(int64_t start, int64_t elements) final {
        auto & buffer = get_buffer();

        // Currently only non-chunked columns are supported
        auto const& typed_array = static_cast<const typename TypeTraits<SrcArrowType>::ArrayType&>(*data->chunk(0));
        typename SrcArrowType::c_type const* data_ptr = typed_array.raw_values();
        typename DestArrowType::c_type* target_ptr =
            reinterpret_cast<typename DestArrowType::c_type*>(buffer.data_pointer());
        std::copy(data_ptr + start, data_ptr + start + elements, target_ptr);

        set_indicator<sizeof(typename DestArrowType::c_type)>(buffer, start, elements);
      }
    };

    struct double_converter : public numeric_converter<DoubleType> {
        double_converter(std::shared_ptr<ChunkedArray> const & data,
                         turbodbc::bound_parameter_set & parameters,
                         std::size_t parameter_index) :
            numeric_converter<DoubleType>(data, parameters, parameter_index, turbodbc::type_code::floating_point)
        { }
    };

    struct int64_converter : public numeric_converter<Int64Type> {
        int64_converter(std::shared_ptr<ChunkedArray> const & data,
                         turbodbc::bound_parameter_set & parameters,
                         std::size_t parameter_index) :
            numeric_converter<Int64Type>(data, parameters, parameter_index, turbodbc::type_code::integer)
        { }
    };

    template <typename ArrowType>
    struct int_converter : public casting_numeric_converter<ArrowType, Int64Type> {
        int_converter(std::shared_ptr<ChunkedArray> const & data,
                      turbodbc::bound_parameter_set & parameters,
                      std::size_t parameter_index) :
            casting_numeric_converter<ArrowType, Int64Type>(data, parameters, parameter_index, turbodbc::type_code::integer)
        { }
    };

    struct bool_converter : public parameter_converter {
        bool_converter(std::shared_ptr<ChunkedArray> const & data,
                         turbodbc::bound_parameter_set & parameters,
                         std::size_t parameter_index) :
            parameter_converter(data, parameters, parameter_index)
        {
          parameters.rebind(parameter_index, turbodbc::make_description(turbodbc::type_code::boolean, 0));
        }

      void set_batch(int64_t start, int64_t elements) final {
        auto & buffer = get_buffer();
        // Currently only non-chunked columns are supported
        auto const& typed_array = static_cast<const BooleanArray&>(*data->chunk(0));
        if (typed_array.null_count() < typed_array.length()) {
          for (int64_t i = 0; i != elements; ++i) {
            if (not typed_array.IsNull(start + i)) {
              buffer.data_pointer()[i] = static_cast<int8_t>(typed_array.Value(start + i));
            }
          }
        }

        set_indicator<sizeof(bool)>(buffer, start, elements);
      };
    };

    struct date_converter : public parameter_converter {
        date_converter(std::shared_ptr<ChunkedArray> const & data,
                         turbodbc::bound_parameter_set & parameters,
                         std::size_t parameter_index) :
            parameter_converter(data, parameters, parameter_index)
        {
          parameters.rebind(parameter_index, turbodbc::make_description(turbodbc::type_code::date, 0));
        }

      void set_batch(int64_t start, int64_t elements) final {
        auto & buffer = get_buffer();
        // Currently only non-chunked columns are supported
        auto const& typed_array = static_cast<const Date32Array&>(*data->chunk(0));
        for (int64_t i = 0; i != elements; ++i) {
          auto element = buffer[i];
          if (not typed_array.IsNull(start + i)) {
            turbodbc::days_to_date(typed_array.Value(start + i), element.data_pointer);
            element.indicator = sizeof(SQL_DATE_STRUCT);
          } else {
            element.indicator = SQL_NULL_DATA;
          }
        }
      };
    };

    struct timestamp_converter : public parameter_converter {
        timestamp_converter(std::shared_ptr<ChunkedArray> const & data,
                         turbodbc::bound_parameter_set & parameters,
                         std::size_t parameter_index) :
            parameter_converter(data, parameters, parameter_index)
        {
          parameters.rebind(parameter_index, turbodbc::make_description(turbodbc::type_code::timestamp, 0));
        }

        virtual void convert(std::int64_t data, char * destination) = 0;

      void set_batch(int64_t start, int64_t elements) final {
        auto & buffer = get_buffer();
        // Currently only non-chunked columns are supported
        auto const& typed_array = static_cast<const TimestampArray&>(*data->chunk(0));
        for (int64_t i = 0; i != elements; ++i) {
          auto element = buffer[i];
          if (not typed_array.IsNull(start + i)) {
            convert(typed_array.Value(start + i), element.data_pointer);
            element.indicator = sizeof(SQL_TIMESTAMP_STRUCT);
          } else {
            element.indicator = SQL_NULL_DATA;
          }
        }
      }
    };

    struct nanosecond_converter : public timestamp_converter {
      using timestamp_converter::timestamp_converter;

        void convert(std::int64_t data, char * destination) final {
            turbodbc::nanoseconds_to_timestamp(data, destination);
        }
    };

    struct microsecond_converter : public timestamp_converter {
      using timestamp_converter::timestamp_converter;

        void convert(std::int64_t data, char * destination) final {
            turbodbc::microseconds_to_timestamp(data, destination);
        }
    };

    std::vector<std::unique_ptr<parameter_converter>> make_converters(
        Table const & table,
        turbodbc::bound_parameter_set & parameters)
    {
        std::vector<std::unique_ptr<parameter_converter>> converters;

        for (int64_t i = 0; i < table.num_columns(); ++i) {
            std::shared_ptr<ChunkedArray> const & data = table.column(i)->data();
            arrow::Type::type dtype = data->type()->id();

            switch (dtype) {
              case arrow::Type::NA:
                converters.emplace_back(new null_converter(data, parameters, i));
              case arrow::Type::INT8:
                converters.emplace_back(new int_converter<Int8Type>(data, parameters, i));
                break;
              case arrow::Type::INT16:
                converters.emplace_back(new int_converter<Int16Type>(data, parameters, i));
                break;
              case arrow::Type::INT32:
                converters.emplace_back(new int_converter<Int32Type>(data, parameters, i));
                break;
              case arrow::Type::INT64:
                converters.emplace_back(new int64_converter(data, parameters, i));
                break;
              case arrow::Type::UINT8:
                converters.emplace_back(new int_converter<UInt8Type>(data, parameters, i));
                break;
              case arrow::Type::UINT16:
                converters.emplace_back(new int_converter<UInt16Type>(data, parameters, i));
                break;
              case arrow::Type::UINT32:
                converters.emplace_back(new int_converter<UInt32Type>(data, parameters, i));
                break;
              case arrow::Type::BINARY:
              case arrow::Type::STRING:
                converters.emplace_back(new string_converter(data, parameters, i));
                break;
              case arrow::Type::DATE32:
                converters.emplace_back(new date_converter(data, parameters, i));
                break;
              case arrow::Type::TIMESTAMP:
                {
                  auto const& time_dtype = static_cast<TimestampType const&>(*data->type());
                  switch (time_dtype.unit()) {
                    case TimeUnit::MICRO:
                      converters.emplace_back(new microsecond_converter(data, parameters, i));
                      break;
                    case TimeUnit::NANO:
                      converters.emplace_back(new nanosecond_converter(data, parameters, i));
                      break;
                    default:
                      throw turbodbc::interface_error("Unsupported timestamp resolution");
                  }
                }
                break;
              case arrow::Type::BOOL:
                converters.emplace_back(new bool_converter(data, parameters, i));
                break;
              case arrow::Type::DOUBLE:
                converters.emplace_back(new double_converter(data, parameters, i));
                break;
              default:
                std::ostringstream message;
                message << "Unsupported Arrow type for column " << (i + 1) << " of ";
                message << table.num_columns() << " (" << data->type()->ToString() << ")";
                throw turbodbc::interface_error(message.str());
            }
        }

        return converters;
    }
}

std::shared_ptr<Table> unwrap_pyarrow_table(pybind11::object const & pyarrow_table) {
    std::shared_ptr<Table> table;
    if (not arrow::py::unwrap_table(pyarrow_table.ptr(), &table).ok()) {
      throw turbodbc::interface_error("Could not unwrap the C++ object from Python pyarrow.Table");
    }
    return table;
}

void assert_table_columns_match_parameters(turbodbc::bound_parameter_set & parameters, Table const& table) {
    if (static_cast<int32_t>(parameters.number_of_parameters()) != table.num_columns()) {
        std::stringstream ss;
        ss << "Number of passed columns (" << table.num_columns();
        ss << ") is not equal to the number of parameters (";
        ss << parameters.number_of_parameters() << ")";
        throw turbodbc::interface_error(ss.str());
    }
}

void set_arrow_parameters(turbodbc::bound_parameter_set & parameters, pybind11::object const & pyarrow_table) {
    arrow::py::import_pyarrow();
    std::shared_ptr<Table> table = unwrap_pyarrow_table(pyarrow_table);
    assert_table_columns_match_parameters(parameters, *table);

    if (table->num_columns() == 0) {
        return;
    }

    auto converters = make_converters(*table, parameters);
    std::size_t const total_sets = static_cast<std::size_t>(table->num_rows());

    for (std::size_t start = 0; start < total_sets; start += parameters.buffered_sets()) {
        auto const in_this_batch = std::min(parameters.buffered_sets(), total_sets - start);
        for (int64_t i = 0; i < table->num_columns(); ++i) {
            converters[i]->set_batch(start, in_this_batch);
        }
        parameters.execute_batch(in_this_batch);
    }
}

}
