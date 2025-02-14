#include <turbodbc_numpy/binary_column.h>
#include <turbodbc_numpy/ndarrayobject.h>
#include <turbodbc_numpy/make_numpy_array.h>

#include <Python.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sql.h>
#include <cstring>

#ifdef __GNUC__
#define EXTENSION __extension__
#else
#define EXTENSION
#endif

namespace turbodbc_numpy {

namespace {

	PyArrayObject * get_array_ptr(pybind11::object & object)
	{
		return reinterpret_cast<PyArrayObject *>(object.ptr());
	}

}

binary_column::binary_column(numpy_type const & type) :
	type_(type),
	data_(make_empty_numpy_array(type)),
	mask_(make_empty_numpy_array(numpy_bool_type)),
	size_(0)
{
}


binary_column::~binary_column() = default;

void binary_column::do_append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values)
{
	auto const old_size = size_;
	resize(old_size + n_values);

	std::memcpy(static_cast<unsigned char *>(PyArray_DATA(get_array_ptr(data_))) + old_size * type_.size,
	            buffer.data_pointer(),
	            n_values * type_.size);

	auto const mask_pointer = static_cast<std::int8_t *>(PyArray_DATA(get_array_ptr(mask_))) + old_size;
	std::memset(mask_pointer, 0, n_values);

	auto const indicator_pointer = buffer.indicator_pointer();
	for (std::size_t element = 0; element != n_values; ++element) {
		if (indicator_pointer[element] == SQL_NULL_DATA) {
			mask_pointer[element] = 1;
		}
	}
}

pybind11::object binary_column::do_get_data()
{
	return data_;
}

pybind11::object binary_column::do_get_mask()
{
	return mask_;
}

void binary_column::resize(std::size_t new_size)
{
	npy_intp size = new_size;
	PyArray_Dims new_dimensions = {&size, 1};
	int const no_reference_check = 0;
	EXTENSION PyArray_Resize(get_array_ptr(data_), &new_dimensions, no_reference_check, NPY_ANYORDER);
	EXTENSION PyArray_Resize(get_array_ptr(mask_), &new_dimensions, no_reference_check, NPY_ANYORDER);
	size_ = new_size;
}


}

