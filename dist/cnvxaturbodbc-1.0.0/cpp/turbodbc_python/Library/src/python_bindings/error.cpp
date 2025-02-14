#include <cpp_odbc/error.h>
#include <turbodbc/errors.h>

#include <pybind11/pybind11.h>

namespace turbodbc { namespace bindings {


void for_error(pybind11::module & module)
{
    pybind11::register_exception<cpp_odbc::error>(module, "Error");
    pybind11::register_exception<turbodbc::interface_error>(module, "InterfaceError");
}

} }
