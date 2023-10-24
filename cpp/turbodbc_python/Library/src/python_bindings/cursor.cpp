#include <turbodbc/cursor.h>

#include <pybind11/pybind11.h>


namespace turbodbc { namespace bindings {

void for_cursor(pybind11::module & module)
{
    pybind11::class_<turbodbc::cursor>(module, "Cursor")
            .def("prepare", &turbodbc::cursor::prepare)
            .def("execute", [](turbodbc::cursor& cursor) {
                pybind11::gil_scoped_release release;
                cursor.execute();
            })
            .def("_reset",  &turbodbc::cursor::reset)
            .def("get_row_count", &turbodbc::cursor::get_row_count)
            .def("get_result_set", &turbodbc::cursor::get_result_set)
        ;
}

} }
