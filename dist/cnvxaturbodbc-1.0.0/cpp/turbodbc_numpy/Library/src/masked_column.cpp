#include <turbodbc_numpy/masked_column.h>

namespace turbodbc_numpy {

masked_column::masked_column() = default;

masked_column::~masked_column() = default;

void masked_column::append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values)
{
	do_append(buffer, n_values);
}

pybind11::object masked_column::get_data()
{
	return do_get_data();
}

pybind11::object masked_column::get_mask()
{
	return do_get_mask();
}

}
