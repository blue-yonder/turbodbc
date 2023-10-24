#include <turbodbc_arrow/arrow_result_set.h>

// Somewhere a macro defines BOOL as a constant. This is in conflict with array/type.h
#undef BOOL
#undef timezone
#include <arrow/api.h>
#include <arrow/python/pyarrow.h>

#include <sql.h>

#include <turbodbc/errors.h>
#include <turbodbc/time_helpers.h>

#include <ciso646>
#include <vector>

#include <boost/locale.hpp>

using arrow::default_memory_pool;
using arrow::AdaptiveIntBuilder;
using arrow::ArrayBuilder;
using arrow::BooleanBuilder;
using arrow::Date32Builder;
using arrow::DoubleBuilder;
using arrow::Int32Builder;
using arrow::Int64Builder;
using arrow::Status;
using arrow::StringBuilder;
using arrow::StringDictionaryBuilder;
using arrow::TimeUnit;
using arrow::TimestampBuilder;
using arrow::Type;

namespace turbodbc_arrow {


namespace {

// We inherit this class to simply provide the same interfaces as StringDictionaryBuilderProxy
//
// Both classes can be removed once we upgrade to an Arrow version where ARROW-4367 is fixed.
class StringBuilderProxy: public StringBuilder {
  public:
    using StringBuilder::StringBuilder;

    Status AppendNullProxy() {
        return AppendNull();
    }

    Status AppendProxy(const char* value, int32_t length) {
        return Append(value, length);
    }
};

class StringDictionaryBuilderProxy: public StringDictionaryBuilder {
  public:
    using StringDictionaryBuilder::StringDictionaryBuilder;

    Status AppendNullProxy() {
        length_++;
        null_count_++;
        return AppendNull();
    }

    Status AppendProxy(const char* value, int32_t length) {
        length_++;
        return Append(value, length);
    }

    Status FinishInternal(std::shared_ptr<arrow::ArrayData>* out) override {
        return StringDictionaryBuilder::FinishInternal(out);
    };

  private:
    size_t length_ = 0;
    size_t null_count_ = 0;
};

std::unique_ptr<ArrayBuilder> make_array_builder(turbodbc::type_code type, bool strings_as_dictionary, bool adaptive_integers)
{
    switch (type) {
        case turbodbc::type_code::floating_point:
            return std::unique_ptr<ArrayBuilder>(new DoubleBuilder());
        case turbodbc::type_code::integer:
            if (adaptive_integers) {
                return std::unique_ptr<ArrayBuilder>(new AdaptiveIntBuilder());
            } else {
                return std::unique_ptr<ArrayBuilder>(new Int64Builder());
            }
        case turbodbc::type_code::boolean:
            return std::unique_ptr<ArrayBuilder>(new BooleanBuilder());
        case turbodbc::type_code::timestamp:
            return std::unique_ptr<TimestampBuilder>(new TimestampBuilder(arrow::timestamp(TimeUnit::MICRO), ::arrow::default_memory_pool()));
        case turbodbc::type_code::date:
            return std::unique_ptr<Date32Builder>(new Date32Builder());
        case turbodbc::type_code::unicode:
            if (strings_as_dictionary) {
                return std::unique_ptr<StringDictionaryBuilderProxy>(new StringDictionaryBuilderProxy(::arrow::utf8(), ::arrow::default_memory_pool()));
            } else {
                return std::unique_ptr<StringBuilderProxy>(new StringBuilderProxy());
            }
        default:
            if (strings_as_dictionary) {
                return std::unique_ptr<StringDictionaryBuilderProxy>(new StringDictionaryBuilderProxy(::arrow::utf8(), ::arrow::default_memory_pool()));
            } else {
                return std::unique_ptr<StringBuilderProxy>(new StringBuilderProxy());
            }
    }
}

template <typename BuilderType>
Status AppendStringsToBuilder(size_t rows_in_batch, BuilderType& builder, cpp_odbc::multi_value_buffer const& input_buffer) {
    for (std::size_t j = 0; j != rows_in_batch; ++j) {
        auto const element = input_buffer[j];
        if (element.indicator == SQL_NULL_DATA) {
            ARROW_RETURN_NOT_OK(builder.AppendNullProxy());
        } else {
            ARROW_RETURN_NOT_OK(builder.AppendProxy(element.data_pointer, element.indicator));
        }
    }
    return Status::OK();
}

template <typename BuilderType>
Status AppendUnicodeStringsToBuilder(size_t rows_in_batch, BuilderType& builder, cpp_odbc::multi_value_buffer const& input_buffer) {
    for (std::size_t j = 0; j != rows_in_batch; ++j) {
        auto const element = input_buffer[j];
        if (element.indicator == SQL_NULL_DATA) {
            ARROW_RETURN_NOT_OK(builder.AppendNullProxy());
        } else {
            std::u16string str_u16(reinterpret_cast<const char16_t*>(element.data_pointer), element.indicator / 2);
            std::string u8string = boost::locale::conv::utf_to_utf<char>(str_u16);;
            ARROW_RETURN_NOT_OK(builder.AppendProxy(u8string.data(), u8string.size()));
        }
    }
    return Status::OK();
}

template <typename BuilderType>
Status AppendIntsToBuilder(size_t rows_in_batch, std::unique_ptr<ArrayBuilder> const& builder, cpp_odbc::multi_value_buffer const& input_buffer, uint8_t* valid_bytes) {
    auto typed_builder = static_cast<BuilderType*>(builder.get());
    auto data_ptr = reinterpret_cast<const int64_t*>(input_buffer.data_pointer());
    return typed_builder->AppendValues(data_ptr, rows_in_batch, valid_bytes);
}

}

arrow_result_set::arrow_result_set(turbodbc::result_sets::result_set & base, bool strings_as_dictionary, bool adaptive_integers) :
    base_result_(base), strings_as_dictionary_(strings_as_dictionary), adaptive_integers_(adaptive_integers)
{
}

std::shared_ptr<arrow::DataType> turbodbc_type_to_arrow(turbodbc::type_code type) {
    switch (type) {
        case turbodbc::type_code::floating_point:
            return arrow::float64();
        case turbodbc::type_code::integer:
            return arrow::int64();
        case turbodbc::type_code::boolean:
            return arrow::boolean();
        case turbodbc::type_code::timestamp:
            return arrow::timestamp(TimeUnit::MICRO);
        case turbodbc::type_code::date:
            return arrow::date32();
        default:
            return std::make_shared<arrow::StringType>();
    }
}

std::shared_ptr<arrow::Schema> arrow_result_set::schema()
{
    auto const column_info = base_result_.get_column_info();
    auto const n_columns = column_info.size();
    std::vector<std::shared_ptr<arrow::Field>> fields;
    for (std::size_t i = 0; i != n_columns; ++i) {
        std::shared_ptr<arrow::DataType> type = turbodbc_type_to_arrow(column_info[i].type);
        fields.emplace_back(std::make_shared<arrow::Field>(column_info[i].name, type, column_info[i].supports_null_values));
    }
    return std::make_shared<arrow::Schema>(fields);
}

Status append_to_double_builder(size_t rows_in_batch, std::unique_ptr<ArrayBuilder> const& builder, cpp_odbc::multi_value_buffer const& input_buffer, uint8_t* valid_bytes) {
    auto typed_builder = static_cast<DoubleBuilder*>(builder.get());
    auto data_ptr = reinterpret_cast<const double*>(input_buffer.data_pointer());
    return typed_builder->AppendValues(data_ptr, rows_in_batch, valid_bytes);
}

Status append_to_int_builder(size_t rows_in_batch, std::unique_ptr<ArrayBuilder> const& builder, cpp_odbc::multi_value_buffer const& input_buffer, uint8_t* valid_bytes, bool adaptive_integers) {
    if (adaptive_integers) {
      return AppendIntsToBuilder<AdaptiveIntBuilder>(rows_in_batch, builder, input_buffer, valid_bytes);
    } else {
      return AppendIntsToBuilder<Int64Builder>(rows_in_batch, builder, input_buffer, valid_bytes);
    }
}

Status append_to_bool_builder(size_t rows_in_batch, std::unique_ptr<ArrayBuilder> const& builder, cpp_odbc::multi_value_buffer const& input_buffer, uint8_t* valid_bytes) {
    auto typed_builder = static_cast<BooleanBuilder*>(builder.get());
    auto data_ptr = reinterpret_cast<const uint8_t*>(input_buffer.data_pointer());
    return typed_builder->AppendValues(data_ptr, rows_in_batch, valid_bytes);
}

Status append_to_timestamp_builder(size_t rows_in_batch, std::unique_ptr<ArrayBuilder> const& builder, cpp_odbc::multi_value_buffer const& input_buffer, uint8_t*) {
    auto typed_builder = static_cast<TimestampBuilder*>(builder.get());
    for (std::size_t j = 0; j < rows_in_batch; ++j) {
        auto element = input_buffer[j];
        if (element.indicator == SQL_NULL_DATA) {
            ARROW_RETURN_NOT_OK(typed_builder->AppendNull());
        } else {
            ARROW_RETURN_NOT_OK(typed_builder->Append(turbodbc::timestamp_to_microseconds(element.data_pointer)));
        }
    }
    return Status::OK();
}

Status append_to_date_builder(size_t rows_in_batch, std::unique_ptr<ArrayBuilder> const& builder, cpp_odbc::multi_value_buffer const& input_buffer, uint8_t*) {
    auto typed_builder = static_cast<Date32Builder*>(builder.get());
    for (std::size_t j = 0; j < rows_in_batch; ++j) {
        auto element = input_buffer[j];
        if (element.indicator == SQL_NULL_DATA) {
            ARROW_RETURN_NOT_OK(typed_builder->AppendNull());
        } else {
            ARROW_RETURN_NOT_OK(typed_builder->Append(turbodbc::date_to_days(element.data_pointer)));
        }
    }
    return Status::OK();
}

Status append_to_string_builder(size_t rows_in_batch, std::unique_ptr<ArrayBuilder> const& builder, cpp_odbc::multi_value_buffer const& input_buffer, uint8_t*, bool strings_as_dictionary) {
    if (strings_as_dictionary) {
        return AppendStringsToBuilder<StringDictionaryBuilderProxy>(rows_in_batch,
            static_cast<StringDictionaryBuilderProxy&>(*builder), input_buffer);
    }

    return AppendStringsToBuilder<StringBuilderProxy>(rows_in_batch,
        static_cast<StringBuilderProxy&>(*builder), input_buffer);
}


Status append_to_unicode_builder(size_t rows_in_batch, std::unique_ptr<ArrayBuilder> const& builder, cpp_odbc::multi_value_buffer const& input_buffer, uint8_t*, bool strings_as_dictionary) {
    if (strings_as_dictionary) {
        return AppendUnicodeStringsToBuilder<StringDictionaryBuilderProxy>(rows_in_batch,
            static_cast<StringDictionaryBuilderProxy&>(*builder), input_buffer);
    }

    return AppendUnicodeStringsToBuilder<StringBuilderProxy>(rows_in_batch,
        static_cast<StringBuilderProxy&>(*builder), input_buffer);
}


Status arrow_result_set::process_batch(size_t rows_in_batch, std::vector<std::unique_ptr<ArrayBuilder>> const& columns) {
    // TODO: Use a PoolBuffer for this and only allocate it once
    auto const column_info = base_result_.get_column_info();
    auto const n_columns = column_info.size();
    std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> const buffers = base_result_.get_buffers();

    std::vector<uint8_t> valid_bytes(rows_in_batch);
    for (size_t i = 0; i != n_columns; ++i) {
        auto const indicator_pointer = buffers[i].get().indicator_pointer();
        for (size_t element = 0; element != rows_in_batch; ++element) {
            if (indicator_pointer[element] == SQL_NULL_DATA) {
                valid_bytes[element] = 0;
            } else {
                valid_bytes[element] = 1;
            }
        }
        switch (column_info[i].type) {
            case turbodbc::type_code::floating_point:
                ARROW_RETURN_NOT_OK(append_to_double_builder(rows_in_batch, columns[i], buffers[i].get(), valid_bytes.data()));
                break;
            case turbodbc::type_code::integer:
                ARROW_RETURN_NOT_OK(append_to_int_builder(rows_in_batch, columns[i], buffers[i].get(), valid_bytes.data(), adaptive_integers_));
                break;
            case turbodbc::type_code::boolean:
                ARROW_RETURN_NOT_OK(append_to_bool_builder(rows_in_batch, columns[i], buffers[i].get(), valid_bytes.data()));
                break;
            case turbodbc::type_code::timestamp:
                ARROW_RETURN_NOT_OK(append_to_timestamp_builder(rows_in_batch, columns[i], buffers[i].get(), valid_bytes.data()));
                break;
            case turbodbc::type_code::date:
                ARROW_RETURN_NOT_OK(append_to_date_builder(rows_in_batch, columns[i], buffers[i].get(), valid_bytes.data()));
                break;
            case turbodbc::type_code::unicode:
                ARROW_RETURN_NOT_OK(append_to_unicode_builder(rows_in_batch, columns[i], buffers[i].get(), valid_bytes.data(), strings_as_dictionary_));
                break;
            default:
                // Strings are the only remaining type
                ARROW_RETURN_NOT_OK(append_to_string_builder(rows_in_batch, columns[i], buffers[i].get(), valid_bytes.data(), strings_as_dictionary_));
                break;
        }
    }
    return Status::OK();
}


Status arrow_result_set::fetch_all_native(std::shared_ptr<arrow::Table>* out, bool single_batch)
{
    std::size_t rows_in_batch = base_result_.fetch_next_batch();
    auto const column_info = base_result_.get_column_info();
    auto const n_columns = column_info.size();

    // Construct the Arrow schema from the SQL schema information
    // We only use this schema for the type information. It does not match the
    // resulting Table as for example String columns may also be encoded as
    // dictionaries.
    std::shared_ptr<arrow::Schema> arrow_schema = schema();

    // Create Builders for all columns
    std::vector<std::unique_ptr<ArrayBuilder>> columns;
    for (std::size_t i = 0; i != n_columns; ++i) {
        columns.push_back(make_array_builder(column_info[i].type, strings_as_dictionary_, adaptive_integers_));
    }

    if (single_batch) {
        ARROW_RETURN_NOT_OK(process_batch(rows_in_batch, columns));
    } else {
        do {
            ARROW_RETURN_NOT_OK(process_batch(rows_in_batch, columns));
            rows_in_batch = base_result_.fetch_next_batch();
        } while (rows_in_batch != 0);
    }

    std::vector<std::shared_ptr<arrow::Array>> arrow_arrays;
    std::vector<std::shared_ptr<arrow::Field>> fields;
    for (size_t i = 0; i != n_columns; ++i) {
        std::shared_ptr<arrow::Array> array;
        ARROW_RETURN_NOT_OK(columns[i]->Finish(&array));
        fields.emplace_back(std::make_shared<arrow::Field>(column_info[i].name, array->type(), column_info[i].supports_null_values));
        arrow_arrays.emplace_back(array);
    }
    // Update to the correct schema, account for e.g. Dictionary columns
    arrow_schema = std::make_shared<arrow::Schema>(fields);
    *out = arrow::Table::Make(arrow_schema, arrow_arrays);

    return Status::OK();
}


pybind11::object arrow_result_set::fetch_next_batch()
{
    std::shared_ptr<arrow::Table> table;
    {
        pybind11::gil_scoped_release release;
        if (not fetch_all_native(&table, true).ok()) {
            throw turbodbc::interface_error("Fetching Arrow result set failed.");
        }
    }

    arrow::py::import_pyarrow();
    return pybind11::reinterpret_steal<pybind11::object>(pybind11::handle(arrow::py::wrap_table(table)));
}


pybind11::object arrow_result_set::fetch_all()
{
    std::shared_ptr<arrow::Table> table;
    {
        pybind11::gil_scoped_release release;
        auto st = fetch_all_native(&table, false);
        if (not st.ok()) {
            throw turbodbc::interface_error("Fetching Arrow result set failed.\n" + st.ToString());
        }
    }
    arrow::py::import_pyarrow();
    auto h = pybind11::handle(arrow::py::wrap_table(table));
    if (!h) {
        throw pybind11::error_already_set();
    }
    return pybind11::reinterpret_steal<pybind11::object>(h);
}


}
