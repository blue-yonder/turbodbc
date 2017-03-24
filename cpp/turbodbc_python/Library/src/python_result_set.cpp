#include <turbodbc_python/python_result_set.h>

#include <turbodbc/make_field_translator.h>

#include <sql.h>
#include <Python.h>
#include <datetime.h> // Python header

#include <ciso646>

namespace turbodbc { namespace result_sets {

namespace {

    using pybind11::reinterpret_steal;
    using pybind11::object;
    using pybind11::cast;

    object make_date(SQL_DATE_STRUCT const & date)
    {
        return reinterpret_steal<object>(PyDate_FromDate(date.year, date.month, date.day));
    }

    object make_timestamp(SQL_TIMESTAMP_STRUCT const & ts)
    {
        // map SQL nanosecond precision to microsecond precision
        int64_t const adjusted_fraction = ts.fraction / 1000;
        return reinterpret_steal<object>(PyDateTime_FromDateAndTime(ts.year, ts.month, ts.day,
                                                                    ts.hour, ts.minute, ts.second, adjusted_fraction));
    }

    object make_object(turbodbc::type_code code, char const * data_pointer, int64_t size)
    {
        switch (code) {
            case type_code::boolean:
                return cast(*reinterpret_cast<bool const *>(data_pointer));
            case type_code::integer:
                return cast(*reinterpret_cast<int64_t const *>(data_pointer));
            case type_code::floating_point:
                return cast(*reinterpret_cast<double const *>(data_pointer));
            case type_code::string:
                return reinterpret_steal<object>(PyUnicode_FromString(data_pointer));
            case type_code::unicode:
                return reinterpret_steal<object>(PyUnicode_DecodeUTF16(data_pointer, size, NULL, NULL));
            case type_code::date:
                return make_date(*reinterpret_cast<SQL_DATE_STRUCT const *>(data_pointer));
            case type_code::timestamp:
                return make_timestamp(*reinterpret_cast<SQL_TIMESTAMP_STRUCT const *>(data_pointer));
            default:
                throw std::logic_error("Encountered unsupported type code");
        }
    }

}


python_result_set::python_result_set(result_set & base) :
    row_based_(base)
{
    PyDateTime_IMPORT;
    for (auto const & info : row_based_.get_column_info()) {
        types_.emplace_back(info.type);
    }
}

std::vector<column_info> python_result_set::get_column_info() const
{
    return row_based_.get_column_info();
}


object python_result_set::fetch_row()
{
    auto const row = row_based_.fetch_row();
    if (not row.empty()) {
        pybind11::list python_row;
        for (std::size_t column = 0; column != row.size(); ++column) {
            if (row[column].indicator == SQL_NULL_DATA) {
                python_row.append(pybind11::none());
            } else {
                python_row.append(make_object(types_[column], row[column].data_pointer, row[column].indicator));
            }
        }
        return python_row;
    } else {
        return pybind11::list();
    }
}



} }
