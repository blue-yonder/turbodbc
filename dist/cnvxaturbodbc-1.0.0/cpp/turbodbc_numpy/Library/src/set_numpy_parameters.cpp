#include <turbodbc_numpy/set_numpy_parameters.h>

#include <turbodbc_numpy/ndarrayobject.h>

#include <turbodbc/errors.h>
#include <turbodbc/make_description.h>
#include <turbodbc/type_code.h>
#include <turbodbc/time_helpers.h>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <utility>
#include <ciso646>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sql.h>

namespace turbodbc_numpy {

namespace {

    struct parameter_converter {
        parameter_converter(pybind11::array const & data,
                            pybind11::array_t<bool> const & mask,
                            turbodbc::bound_parameter_set & parameters,
                            std::size_t parameter_index) :
            data(data),
            mask(mask),
            parameters(parameters),
            parameter_index(parameter_index)
        {}

        cpp_odbc::multi_value_buffer & get_buffer() {
            return parameters.get_parameters()[parameter_index]->get_buffer();
        }
        virtual void set_batch(std::size_t start, std::size_t elements) = 0;

        virtual ~parameter_converter() = default;

        pybind11::array const & data;
        pybind11::array_t<bool> const & mask;
        turbodbc::bound_parameter_set & parameters;
        std::size_t const parameter_index;
    };

    template <typename Value>
    struct binary_converter : public parameter_converter {
        binary_converter(pybind11::array const & data, pybind11::array_t<bool> const & mask,
                         turbodbc::bound_parameter_set & parameters,
                         std::size_t parameter_index,
                         turbodbc::type_code type) :
            parameter_converter(data, mask, parameters, parameter_index),
            type(type)
        {
            parameters.rebind(parameter_index, turbodbc::make_description(type, 0));
        }

        void set_batch(std::size_t start, std::size_t elements) final
        {
            auto data_ptr = data.unchecked<Value, 1>().data(0);
            auto & buffer = get_buffer();
            std::memcpy(buffer.data_pointer(), data_ptr + start, elements * sizeof(Value));
            if (mask.size() != 1) {
                auto const indicator = buffer.indicator_pointer();
                auto const mask_start = mask.unchecked<1>().data(start);
                for (std::size_t i = 0; i != elements; ++i) {
                    indicator[i] = (mask_start[i] == NPY_TRUE) ? SQL_NULL_DATA : sizeof(Value);
                }
            } else {
                intptr_t const sql_mask = (*mask.data() == NPY_TRUE) ? SQL_NULL_DATA : sizeof(Value);
                std::fill_n(buffer.indicator_pointer(), elements, sql_mask);
            }
        }
    private:
        turbodbc::type_code type;
    };


    struct datetime64_converter : public parameter_converter {
        datetime64_converter(pybind11::array const & data,
                             pybind11::array_t<bool> const & mask,
                             turbodbc::bound_parameter_set & parameters,
                             std::size_t parameter_index,
                             turbodbc::type_code code,
                             std::intptr_t element_size) :
            parameter_converter(data, mask, parameters, parameter_index),
            uses_individual_mask(mask.size() != 1),
            element_size(element_size)
        {
            parameters.rebind(parameter_index, turbodbc::make_description(code, 0));
        }

        void set_batch_with_individual_mask(std::size_t start, std::size_t elements)
        {
            auto & buffer = get_buffer();
            auto const data_start = data.unchecked<std::int64_t, 1>().data(start);
            auto const mask_start = mask.unchecked<1>().data(start);

            for (std::size_t i = 0; i != elements; ++i) {
                auto element = buffer[i];
                if (mask_start[i] == NPY_TRUE) {
                    element.indicator = SQL_NULL_DATA;
                } else {
                    convert(data_start[i], element.data_pointer);
                    element.indicator = element_size;
                }
            }
        }

        void set_batch_with_shared_mask(std::size_t start, std::size_t elements)
        {
            auto & buffer = get_buffer();
            auto const data_start = data.unchecked<std::int64_t, 1>().data(start);

            if (*mask.data() == NPY_TRUE) {
                std::fill_n(buffer.indicator_pointer(), elements, static_cast<std::int64_t>(SQL_NULL_DATA));
            } else {
                for (std::size_t i = 0; i != elements; ++i) {
                    auto element = buffer[i];
                    convert(data_start[i], element.data_pointer);
                    element.indicator = element_size;
                }
            }
        }

        void set_batch(std::size_t start, std::size_t elements) final
        {
            if (uses_individual_mask) {
                set_batch_with_individual_mask(start, elements);
            } else {
                set_batch_with_shared_mask(start, elements);
            }
        }

        virtual void convert(std::int64_t data, char * destination) = 0;
    private:
        bool const uses_individual_mask;
        std::intptr_t element_size;
    };


    struct microseconds_converter : public datetime64_converter {
        microseconds_converter(pybind11::array const & data,
                            pybind11::array_t<bool> const & mask,
                            turbodbc::bound_parameter_set & parameters,
                            std::size_t parameter_index) :
            datetime64_converter(data,
                                 mask,
                                 parameters,
                                 parameter_index,
                                 turbodbc::type_code::timestamp,
                                 sizeof(SQL_TIMESTAMP_STRUCT))
        {}

        virtual void convert(std::int64_t data, char * destination) {
            turbodbc::microseconds_to_timestamp(data, destination);
        }
    };


    struct nanoseconds_converter : public datetime64_converter {
        nanoseconds_converter(pybind11::array const & data,
                              pybind11::array_t<bool> const & mask,
                              turbodbc::bound_parameter_set & parameters,
                              std::size_t parameter_index) :
            datetime64_converter(data,
                                 mask,
                                 parameters,
                                 parameter_index,
                                 turbodbc::type_code::timestamp,
                                 sizeof(SQL_TIMESTAMP_STRUCT))
        {}

        virtual void convert(std::int64_t data, char * destination) {
            turbodbc::nanoseconds_to_timestamp(data, destination);
        }
    };


    struct date_converter : public datetime64_converter {
        date_converter(pybind11::array const & data,
                       pybind11::array_t<bool> const & mask,
                       turbodbc::bound_parameter_set & parameters,
                       std::size_t parameter_index) :
            datetime64_converter(data,
                                 mask,
                                 parameters,
                                 parameter_index,
                                 turbodbc::type_code::date,
                                 sizeof(SQL_DATE_STRUCT))
        {}

        virtual void convert(std::int64_t data, char * destination) {
            turbodbc::days_to_date(data, destination);
        }
    };


    struct string_converter : public parameter_converter {
        string_converter(pybind11::array const & data,
                         pybind11::array_t<bool> const & mask,
                         turbodbc::bound_parameter_set & parameters,
                         std::size_t parameter_index) :
            parameter_converter(data, mask, parameters, parameter_index),
            type(parameters.get_initial_parameter_types()[parameter_index])
        {}

        template <typename String>
        std::vector<std::pair<bool, String>> extract_batch_with_individual_mask(std::size_t start, std::size_t elements)
        {
            auto data_start = data.unchecked<pybind11::object, 1>().data(start);
            auto mask_start = mask.unchecked<1>().data(start);

            std::vector<std::pair<bool, String>> batch;
            batch.reserve(elements);

            for (std::size_t i = 0; i != elements; ++i) {
                if ((mask_start[i] == NPY_TRUE) or (data_start[i].is_none())) {
                    batch.push_back(std::pair<bool, String>{true, String()});
                } else {
                    batch.push_back(std::pair<bool, String>{false, pybind11::cast<String>(data_start[i])});
                }
            }
            return batch;
        }

        template <typename String>
        std::vector<std::pair<bool, String>> extract_batch_with_shared_mask(std::size_t start, std::size_t elements)
        {
            auto data_start = data.unchecked<pybind11::object, 1>().data(start);
            bool const all_masked = (*mask.data() == NPY_TRUE);

            std::vector<std::pair<bool, String>> batch;
            batch.reserve(elements);

            for (std::size_t i = 0; i != elements; ++i) {
                if (all_masked or (data_start[i].is_none())) {
                    batch.push_back(std::pair<bool, String>{true, String()});
                } else {
                    batch.push_back(std::pair<bool, String>{false, pybind11::cast<String>(data_start[i])});
                }
            }
            return batch;
        }

        template <typename String>
        std::size_t maximum_string_length(std::vector<std::pair<bool, String>> const & batch)
        {
            auto const & largest_string = *std::max_element(batch.begin(),
                                                            batch.end(),
                                                            [](std::pair<bool, String> const & a, std::pair<bool, String> const & b){
                                                                return a.second.size() < b.second.size();
                                                            });
            return largest_string.second.size();
        }


        template <typename String>
        void fill_batch(std::vector<std::pair<bool, String>> const & batch)
        {
            auto const maximum_length = maximum_string_length(batch);
            // Propagate the maximum string length to the parameters.
            // These then adjust the size of the underlying buffer.
            parameters.rebind(parameter_index, turbodbc::make_description(type, maximum_length));
            auto & buffer = get_buffer();
            auto const character_size = sizeof(typename String::value_type);

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

        template <typename String>
        void set_batch_of_type(std::size_t start, std::size_t elements)
        {
            if (mask.size() != 1) {
                auto const batch = extract_batch_with_individual_mask<String>(start, elements);
                fill_batch(batch);
            } else {
                auto const batch = extract_batch_with_shared_mask<String>(start, elements);
                fill_batch(batch);
            }
        }

        void set_batch(std::size_t start, std::size_t elements) final
        {
            if (type == turbodbc::type_code::unicode) {
                set_batch_of_type<std::u16string>(start, elements);
            } else {
                set_batch_of_type<std::string>(start, elements);
            }
        }
    private:
        turbodbc::type_code type;
    };


    std::vector<std::unique_ptr<parameter_converter>> make_converters(
        std::vector<std::tuple<pybind11::array, pybind11::array_t<bool>, std::string>> const & columns,
        turbodbc::bound_parameter_set & parameters)
    {
        std::vector<std::unique_ptr<parameter_converter>> converters;

        for (std::size_t i = 0; i != columns.size(); ++i) {
            auto const & data = std::get<0>(columns[i]);
            auto const & mask = std::get<1>(columns[i]);
            auto const & dtype = std::get<2>(columns[i]);
            if (dtype == "int64") {
               converters.emplace_back(new binary_converter<std::int64_t>(data, mask, parameters, i, turbodbc::type_code::integer));
            } else if (dtype == "float64") {
                converters.emplace_back(new binary_converter<double>(data, mask, parameters, i, turbodbc::type_code::floating_point));
            } else if (dtype == "datetime64[us]") {
                converters.emplace_back(new microseconds_converter(data, mask, parameters, i));
            } else if (dtype == "datetime64[ns]") {
                converters.emplace_back(new nanoseconds_converter(data, mask, parameters, i));
            } else if (dtype == "datetime64[D]") {
                converters.emplace_back(new date_converter(data, mask, parameters, i));
            } else if (dtype == "bool") {
                converters.emplace_back(new binary_converter<std::int8_t>(data, mask, parameters, i, turbodbc::type_code::boolean));
            } else if (dtype == "object") {
                converters.emplace_back(new string_converter(data, mask, parameters, i));
            } else {
                std::ostringstream message;
                message << "Unsupported NumPy dtype for column " << (i + 1) << " of " << columns.size();
                message << " (unsupported type: " << dtype << ")";
                throw turbodbc::interface_error(message.str());
            }
        }

        return converters;
    }

}

void set_numpy_parameters(turbodbc::bound_parameter_set & parameters, std::vector<std::tuple<pybind11::array, pybind11::array_t<bool>, std::string>> const & columns)
{
    if (parameters.number_of_parameters() != columns.size()) {
        throw turbodbc::interface_error("Number of passed columns is not equal to the number of parameters");
    }

    if (columns.size() == 0) {
        return;
    }

    auto converters = make_converters(columns, parameters);

    std::size_t total_sets = static_cast<std::size_t>(std::get<0>(columns.front()).size());

    for (std::size_t start = 0; start < total_sets; start += parameters.buffered_sets()) {
        auto const in_this_batch = std::min(parameters.buffered_sets(), total_sets - start);
        for (std::size_t i = 0; i != columns.size(); ++i) {
            converters[i]->set_batch(start, in_this_batch);
        }
        parameters.execute_batch(in_this_batch);
    }
}

}
