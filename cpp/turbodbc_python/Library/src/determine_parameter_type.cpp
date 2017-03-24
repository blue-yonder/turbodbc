#include <turbodbc_python/determine_parameter_type.h>

#include <stdexcept>

#include <datetime.h> // Python header


namespace turbodbc {

namespace {

    struct datetime_initializer {
        datetime_initializer()
        {
            PyDateTime_IMPORT;
        }
    };

    static const datetime_initializer required_for_datetime_interaction;

    std::size_t const size_not_important = 0;

    void set_integer(pybind11::handle const & value, cpp_odbc::writable_buffer_element & destination)
    {
        *reinterpret_cast<int64_t *>(destination.data_pointer) = value.cast<int64_t>();
        destination.indicator = sizeof(int64_t);
    }

    void set_floating_point(pybind11::handle const & value, cpp_odbc::writable_buffer_element & destination)
    {
        *reinterpret_cast<double *>(destination.data_pointer) = value.cast<double>();
        destination.indicator = sizeof(double);
    }

    void set_string(pybind11::handle const & value, cpp_odbc::writable_buffer_element & destination)
    {
        auto const s = value.cast<std::string>();
        auto const length_with_null_termination = s.size() + 1;
        std::memcpy(destination.data_pointer, s.c_str(), length_with_null_termination);
        destination.indicator = s.size();
    }

    void set_unicode(pybind11::handle const & value, cpp_odbc::writable_buffer_element & destination)
    {
        auto const s = value.cast<std::u16string>();
        auto const length_with_null_termination = 2 * (s.size() + 1);
        std::memcpy(destination.data_pointer, s.c_str(), length_with_null_termination);
        destination.indicator = 2 * s.size();
    }

    void set_date(pybind11::handle const & value, cpp_odbc::writable_buffer_element & destination)
    {
        auto ptr = value.ptr();
        auto d = reinterpret_cast<SQL_DATE_STRUCT *>(destination.data_pointer);

        d->year = PyDateTime_GET_YEAR(ptr);
        d->month = PyDateTime_GET_MONTH(ptr);
        d->day = PyDateTime_GET_DAY(ptr);

        destination.indicator = sizeof(SQL_DATE_STRUCT);
    }

    void set_timestamp(pybind11::handle const & value, cpp_odbc::writable_buffer_element & destination)
    {
        auto ptr = value.ptr();
        auto d = reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(destination.data_pointer);

        d->year = PyDateTime_GET_YEAR(ptr);
        d->month = PyDateTime_GET_MONTH(ptr);
        d->day = PyDateTime_GET_DAY(ptr);
        d->hour = PyDateTime_DATE_GET_HOUR(ptr);
        d->minute = PyDateTime_DATE_GET_MINUTE(ptr);
        d->second = PyDateTime_DATE_GET_SECOND(ptr);
        // map microsecond precision to SQL nanosecond precision
        d->fraction = PyDateTime_DATE_GET_MICROSECOND(ptr) * 1000;

        destination.indicator = sizeof(SQL_TIMESTAMP_STRUCT);
    }

}


python_parameter_info determine_parameter_type(pybind11::handle const & value, type_code initial_type)
{
    {
        auto caster = pybind11::detail::make_caster<int64_t>();
        if (caster.load(value, true)) {
            return {set_integer, type_code::integer, size_not_important};
        }
    }
    {
        auto caster = pybind11::detail::make_caster<double>();
        if (caster.load(value, true)) {
            return {set_floating_point, type_code::floating_point, size_not_important};
        }
    }
    if (initial_type == type_code::unicode) {
        auto caster = pybind11::detail::make_caster<std::u16string>();
        if (caster.load(value, true)) {
            auto const temp = value.cast<std::u16string>();
            return {set_unicode, type_code::unicode, temp.size()};
        }
    } else {
        auto caster = pybind11::detail::make_caster<std::string>();
        if (caster.load(value, true)) {
            auto const temp = value.cast<std::string>();
            return {set_string, type_code::string, temp.size()};
        }
    }

    auto ptr = value.ptr();
    if (PyDateTime_Check(ptr)) {
        return {set_timestamp, type_code::timestamp, size_not_important};
    }

    if (PyDate_Check(ptr)) {
        return {set_date, type_code::date, size_not_important};
    }

    throw std::runtime_error("Could not convert python value to C++");
}


}