#include <turbodbc_numpy/string_column.h>

#include <turbodbc/string_helpers.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sql.h>
#include <cstring>

namespace turbodbc_numpy {

using pybind11::object;
using pybind11::reinterpret_steal;

string_column::string_column(std::size_t total_buffer_size) :
    maximum_text_size_(total_buffer_size - 1)
{
}


string_column::~string_column() = default;

void string_column::do_append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values)
{
    for (std::size_t i = 0; i != n_values; ++i) {
        auto const element = buffer[i];
        if (element.indicator == SQL_NULL_DATA) {
            data_.append(pybind11::none());
        } else {
            auto const size = turbodbc::buffered_string_size(element.indicator, maximum_text_size_);
            data_.append(reinterpret_steal<object>(PyUnicode_DecodeUTF8(element.data_pointer, size, "ignore")));
        }
    }
}

object string_column::do_get_data()
{
    return data_;
}

object string_column::do_get_mask()
{
    return pybind11::cast(false);
}


}

