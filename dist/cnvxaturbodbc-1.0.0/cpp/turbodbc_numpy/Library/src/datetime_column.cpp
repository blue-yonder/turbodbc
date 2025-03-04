#include <turbodbc_numpy/datetime_column.h>
#include <turbodbc_numpy/ndarrayobject.h>
#include <turbodbc_numpy/make_numpy_array.h>
#include <turbodbc/time_helpers.h>

#include <Python.h>

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

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

	std::string get_type_descriptor(turbodbc::type_code type)
	{
		if (type == turbodbc::type_code::timestamp) {
			return "datetime64[us]";
		} else {
			return "datetime64[D]";
		}
	}

	datetime_column::converter make_converter(turbodbc::type_code type)
	{
		if (type == turbodbc::type_code::timestamp) {
			return turbodbc::timestamp_to_microseconds;
		} else {
			return turbodbc::date_to_days;
		}
	}

	PyArrayObject * get_array_ptr(pybind11::object & object)
	{
		return reinterpret_cast<PyArrayObject *>(object.ptr());
	}

}

datetime_column::datetime_column(turbodbc::type_code type) :
	type_(type),
	data_(make_empty_numpy_array(get_type_descriptor(type_))),
	mask_(make_empty_numpy_array(numpy_bool_type)),
	size_(0),
	converter_(make_converter(type_))
{
}


datetime_column::~datetime_column() = default;

void datetime_column::do_append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values)
{
	auto const old_size = size_;
	resize(old_size + n_values);

	auto const data_pointer = static_cast<int64_t *>(PyArray_DATA(get_array_ptr(data_))) + old_size;
	auto const mask_pointer = static_cast<std::int8_t *>(PyArray_DATA(get_array_ptr(mask_))) + old_size;
	std::memset(mask_pointer, 0, n_values);

	for (std::size_t i = 0; i != n_values; ++i) {
		auto element = buffer[i];
		if (element.indicator == SQL_NULL_DATA) {
			mask_pointer[i] = 1;
		} else {
			reinterpret_cast<intptr_t *>(data_pointer)[i] = converter_(element.data_pointer);
		}
	}
}

pybind11::object datetime_column::do_get_data()
{
	return data_;
}

pybind11::object datetime_column::do_get_mask()
{
	return mask_;
}

void datetime_column::resize(std::size_t new_size)
{
	npy_intp size = new_size;
	PyArray_Dims new_dimensions = {&size, 1};
	int const no_reference_check = 0;
	EXTENSION PyArray_Resize(get_array_ptr(data_), &new_dimensions, no_reference_check, NPY_ANYORDER);
	EXTENSION PyArray_Resize(get_array_ptr(mask_), &new_dimensions, no_reference_check, NPY_ANYORDER);
	size_ = new_size;
}


}

