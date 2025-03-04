#include <turbodbc/buffer_size.h>

#include <pybind11/pybind11.h>

// for the conversion of turbodbc::buffer_size, please see
// python_bindings/connection.cpp. There is no global converter
// registry, so it is defined at the only place where it is
// needed.

namespace turbodbc { namespace bindings {

void for_buffer_size(pybind11::module & module)
{
    pybind11::class_<turbodbc::rows>(module, "Rows")
		.def(pybind11::init<std::size_t>())
		.def_readwrite("rows", &turbodbc::rows::value)
    ;

    pybind11::class_<turbodbc::megabytes>(module, "Megabytes")
		.def(pybind11::init<std::size_t>())
		.def_readwrite("megabytes", &turbodbc::megabytes::value)
    ;

}

} }