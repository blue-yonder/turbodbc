#include <turbodbc/connection.h>

#include <pybind11/pybind11.h>


namespace turbodbc { namespace bindings {

void for_connection(pybind11::module &module) {
    pybind11::class_<turbodbc::connection>(module, "Connection")
        .def("commit", &turbodbc::connection::commit, pybind11::call_guard<pybind11::gil_scoped_release>())
        .def("rollback", &turbodbc::connection::rollback, pybind11::call_guard<pybind11::gil_scoped_release>())
        .def("cursor", &turbodbc::connection::make_cursor, pybind11::call_guard<pybind11::gil_scoped_release>())
        .def("set_autocommit", &turbodbc::connection::set_autocommit, pybind11::call_guard<pybind11::gil_scoped_release>())
        .def("autocommit_enabled", &turbodbc::connection::autocommit_enabled)
        ;

}

} }
