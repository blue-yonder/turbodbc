#include <turbodbc_python/python_result_set.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


using base_result_set = turbodbc::result_sets::result_set;
using turbodbc::result_sets::python_result_set;

namespace turbodbc { namespace bindings {

python_result_set make_python_result_set(std::shared_ptr<base_result_set> result_set_pointer)
{
	return python_result_set(*result_set_pointer);
}


void for_python_result_set(pybind11::module & module)
{
	// expose base result set with explicit holder class to allow passing of
	// shared pointer arguments
	pybind11::class_<base_result_set, std::shared_ptr<base_result_set>>(module, "BaseResultSet");

	pybind11::class_<python_result_set>(module, "ResultSet")
			.def("get_column_info", &python_result_set::get_column_info)
			.def("fetch_row", &python_result_set::fetch_row)
		;

	module.def("make_row_based_result_set", make_python_result_set);
}

} }
