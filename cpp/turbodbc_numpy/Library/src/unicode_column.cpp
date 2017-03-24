#include <turbodbc_numpy/unicode_column.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sql.h>
#include <cstring>

namespace turbodbc_numpy {

using pybind11::object;
using pybind11::reinterpret_steal;

unicode_column::unicode_column()
{
}


unicode_column::~unicode_column() = default;

void unicode_column::do_append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values)
{
	for (std::size_t i = 0; i != n_values; ++i) {
		auto const element = buffer[i];
		if (element.indicator == SQL_NULL_DATA) {
			data_.append(pybind11::none());
		} else {
			data_.append(reinterpret_steal<object>(PyUnicode_DecodeUTF16(element.data_pointer, element.indicator, NULL, NULL)));
		}
	}
}

object unicode_column::do_get_data()
{
	return data_;
}

object unicode_column::do_get_mask()
{
	return pybind11::cast(false);
}


}

