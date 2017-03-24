#pragma once

#include <turbodbc/parameter.h>
#include <cpp_odbc/multi_value_buffer.h>

#include <pybind11/pybind11.h>


namespace turbodbc {

struct python_parameter_info {
	using parameter_converter = void(*)(pybind11::handle const &, cpp_odbc::writable_buffer_element &);

	parameter_converter converter;
	type_code type;
	std::size_t size;
};

python_parameter_info determine_parameter_type(pybind11::handle const & value, type_code initial_type);

}