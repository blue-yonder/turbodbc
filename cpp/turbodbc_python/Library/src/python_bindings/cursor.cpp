#include <turbodbc/cursor.h>

#include <pybind11/pybind11.h>


namespace turbodbc { namespace bindings {

void for_cursor(pybind11::module & module)
{
    pybind11::class_<turbodbc::cursor>(module, "Cursor")
            .def("prepare", &turbodbc::cursor::prepare, pybind11::call_guard<pybind11::gil_scoped_release>())
            .def("prepare_with_timeout", &turbodbc::cursor::prepare_with_timeout,
                pybind11::call_guard<pybind11::gil_scoped_release>(),
                pybind11::arg("sql"),
                pybind11::arg("timeout_seconds") = 0,
                "Prepare the given SQL with a custom statement timeout (seconds)")
            .def("execute", &turbodbc::cursor::execute, pybind11::call_guard<pybind11::gil_scoped_release>())
            .def("_reset",  &turbodbc::cursor::reset, pybind11::call_guard<pybind11::gil_scoped_release>())
            .def("get_row_count", &turbodbc::cursor::get_row_count)
            .def("get_result_set", &turbodbc::cursor::get_result_set)
            .def("more_results", &turbodbc::cursor::more_results)
        ;
}

} }
