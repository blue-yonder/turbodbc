#include <turbodbc_arrow/arrow_result_set.h>
#include <turbodbc_arrow/set_arrow_parameters.h>
#include <turbodbc_numpy/numpy_result_set.h>
#include <turbodbc_numpy/set_numpy_parameters.h>
#include <turbodbc/cursor.h>

#define PY_ARRAY_UNIQUE_SYMBOL turbodbc_numpy_API
#include <numpy/ndarrayobject.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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

}
namespace result_sets {
    void python_result_set_init();
}
    void determine_parameter_type_init();
}

namespace {

turbodbc_arrow::arrow_result_set make_arrow_result_set(std::shared_ptr<turbodbc::result_sets::result_set> result_set_pointer,
    bool strings_as_dictionary, bool adaptive_integers)
{
    return turbodbc_arrow::arrow_result_set(*result_set_pointer, strings_as_dictionary, adaptive_integers);
}

void set_arrow_parameters(turbodbc::cursor & cursor, pybind11::object const & pyarrow_table)
{
    turbodbc_arrow::set_arrow_parameters(cursor.get_command()->get_parameters(), pyarrow_table);
}

turbodbc_numpy::numpy_result_set make_numpy_result_set(std::shared_ptr<turbodbc::result_sets::result_set> result_set_pointer)
{
    return turbodbc_numpy::numpy_result_set(*result_set_pointer);
}

void set_numpy_parameters(turbodbc::cursor & cursor, std::vector<std::tuple<pybind11::array, pybind11::array_t<bool>, std::string>> const & columns)
{
    turbodbc_numpy::set_numpy_parameters(cursor.get_command()->get_parameters(), columns);
}

}

using namespace turbodbc;

void * enable_numpy_support()
{
    import_array();
    return nullptr;
}

PYBIND11_MODULE(turbodbc_intern, module)
{
    enable_numpy_support();
    result_sets::python_result_set_init();
    determine_parameter_type_init();

    module.doc() = "Native helpers for the turbodbc package";
    bindings::for_buffer_size(module);
    bindings::for_column_info(module);
    bindings::for_connect(module);
    bindings::for_connection(module);
    bindings::for_cursor(module);
    bindings::for_error(module);
    bindings::for_options(module);
    bindings::for_python_result_set(module);
    bindings::for_python_parameter_set(module);

    pybind11::class_<turbodbc_arrow::arrow_result_set>(module, "ArrowResultSet")
        .def("fetch_all", &turbodbc_arrow::arrow_result_set::fetch_all)
        .def("fetch_next_batch", &turbodbc_arrow::arrow_result_set::fetch_next_batch);

    module.def("make_arrow_result_set", make_arrow_result_set);
    module.def("set_arrow_parameters", set_arrow_parameters);

    pybind11::class_<turbodbc_numpy::numpy_result_set>(module, "NumpyResultSet")
        .def("fetch_next_batch", &turbodbc_numpy::numpy_result_set::fetch_next_batch);

    module.def("make_numpy_result_set", make_numpy_result_set);
    module.def("set_numpy_parameters", set_numpy_parameters);
}
