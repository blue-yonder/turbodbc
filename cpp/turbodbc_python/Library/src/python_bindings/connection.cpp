#include <turbodbc/connection.h>

#include <pybind11/pybind11.h>


namespace turbodbc { namespace bindings {

void for_connection(pybind11::module &module) {
    pybind11::class_<turbodbc::connection>(module, "Connection")
        .def("commit", &turbodbc::connection::commit)
        .def("rollback", &turbodbc::connection::rollback)
        .def("cursor", &turbodbc::connection::make_cursor)
        .def("set_autocommit", &turbodbc::connection::set_autocommit)
        .def("autocommit_enabled", &turbodbc::connection::autocommit_enabled)
        ;

}

} }
