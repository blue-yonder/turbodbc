#include <turbodbc_python/python_parameter_set.h>
#include <turbodbc/cursor.h>

#include <pybind11/pybind11.h>


namespace turbodbc { namespace bindings {


python_parameter_set make_parameter_set(cursor & cursor)
{
	return {cursor.get_command()->get_parameters()};
}


void for_python_parameter_set(pybind11::module & module)
{
	pybind11::class_<python_parameter_set>(module, "ParameterSet")
			.def("add_set", &python_parameter_set::add_parameter_set)
			.def("flush", &python_parameter_set::flush)
		;

	module.def("make_parameter_set", make_parameter_set);
}

} }
