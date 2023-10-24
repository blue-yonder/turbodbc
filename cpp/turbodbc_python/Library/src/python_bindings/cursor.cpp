#include <turbodbc/cursor.h>

#include <pybind11/pybind11.h>


namespace turbodbc { namespace bindings {

void for_cursor(pybind11::module & module)
{
    pybind11::class_<turbodbc::cursor>(module, "Cursor")
            .def("prepare", &turbodbc::cursor::prepare, pybind11::call_guard<pybind11::gil_scoped_release>())
            .def("execute", &turbodbc::cursor::execute, pybind11::call_guard<pybind11::gil_scoped_release>())
            .def("_reset",  &turbodbc::cursor::reset, pybind11::call_guard<pybind11::gil_scoped_release>())
            .def("get_row_count", &turbodbc::cursor::get_row_count)
            .def("get_result_set", &turbodbc::cursor::get_result_set)
            .def("more_results", &turbodbc::cursor::more_results)
        ;
}

} }
