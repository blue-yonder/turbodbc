#include <turbodbc/column_info.h>

#include <pybind11/pybind11.h>


using turbodbc::column_info;

namespace turbodbc { namespace bindings {

void for_column_info(pybind11::module & module)
{
    pybind11::class_<column_info>(module, "ColumnInfo")
        .def_readonly("name", &column_info::name)
        .def_readonly("supports_null_values", &column_info::supports_null_values)
        .def("type_code", [](column_info const & info) {
            return static_cast<int>(info.type);
        });
}

} }


