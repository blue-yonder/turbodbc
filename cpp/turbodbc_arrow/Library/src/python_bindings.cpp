#include <turbodbc_arrow/arrow_result_set.h>
#include <turbodbc_arrow/set_arrow_parameters.h>
#include <turbodbc/cursor.h>

#include <pybind11/pybind11.h>

using turbodbc_arrow::arrow_result_set;

namespace {

arrow_result_set make_arrow_result_set(std::shared_ptr<turbodbc::result_sets::result_set> result_set_pointer,
    bool strings_as_dictionary, bool adaptive_integers, bool truncate_timestamps)
{
	return arrow_result_set(*result_set_pointer, strings_as_dictionary, adaptive_integers, truncate_timestamps);
}

void set_arrow_parameters(turbodbc::cursor & cursor, pybind11::object const & pyarrow_table)
{
    turbodbc_arrow::set_arrow_parameters(cursor.get_command()->get_parameters(), pyarrow_table);
}

}

PYBIND11_MODULE(turbodbc_arrow_support, module)
{
    module.doc() = "Native helpers for turbodbc's Apache Arrow support";

    pybind11::class_<arrow_result_set>(module, "ArrowResultSet")
        .def("fetch_all", &arrow_result_set::fetch_all)
        .def("fetch_next_batch", &arrow_result_set::fetch_next_batch);

    module.def("make_arrow_result_set", make_arrow_result_set);
    module.def("set_arrow_parameters", set_arrow_parameters);
}
