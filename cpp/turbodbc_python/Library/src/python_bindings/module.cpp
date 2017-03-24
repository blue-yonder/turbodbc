#include <pybind11/pybind11.h>

namespace turbodbc { namespace bindings {

    void for_buffer_size(pybind11::module &);
    void for_column_info(pybind11::module &);
    void for_connect(pybind11::module &);
    void for_connection(pybind11::module &);
    void for_cursor(pybind11::module &);
    void for_error(pybind11::module &);
    void for_options(pybind11::module &);
    void for_python_result_set(pybind11::module &);
    void for_python_parameter_set(pybind11::module &);

} }


using namespace turbodbc;

PYBIND11_PLUGIN(turbodbc_intern)
{
    pybind11::module module("turbodbc_intern", "Native helpers for the turbodbc package");
    bindings::for_buffer_size(module);
    bindings::for_column_info(module);
    bindings::for_connect(module);
    bindings::for_connection(module);
    bindings::for_cursor(module);
    bindings::for_error(module);
    bindings::for_options(module);
    bindings::for_python_result_set(module);
    bindings::for_python_parameter_set(module);
    return module.ptr();
}
