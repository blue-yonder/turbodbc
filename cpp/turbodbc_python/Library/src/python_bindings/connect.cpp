#include <turbodbc/connect.h>

#include <pybind11/pybind11.h>

namespace turbodbc { namespace bindings {

void for_connect(pybind11::module & module)
{
    module.def("connect", turbodbc::connect);
}

} }
