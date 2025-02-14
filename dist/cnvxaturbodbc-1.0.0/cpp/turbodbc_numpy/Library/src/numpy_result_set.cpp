#include <turbodbc_numpy/numpy_result_set.h>

#include <turbodbc_numpy/numpy_type.h>
#include <turbodbc_numpy/binary_column.h>
#include <turbodbc_numpy/datetime_column.h>
#include <turbodbc_numpy/string_column.h>
#include <turbodbc_numpy/unicode_column.h>

#include <vector>


namespace turbodbc_numpy {


namespace {

    std::unique_ptr<masked_column> make_masked_column(turbodbc::column_info const & info)
    {
        switch (info.type) {
            case turbodbc::type_code::floating_point:
                return std::unique_ptr<binary_column>(new binary_column(numpy_double_type));
            case turbodbc::type_code::integer:
                return std::unique_ptr<binary_column>(new binary_column(numpy_int_type));
            case turbodbc::type_code::boolean:
                return std::unique_ptr<binary_column>(new binary_column(numpy_bool_type));
            case turbodbc::type_code::timestamp:
            case turbodbc::type_code::date:
                return std::unique_ptr<datetime_column>(new datetime_column(info.type));
            case turbodbc::type_code::unicode:
                return std::unique_ptr<unicode_column>(new unicode_column(info.element_size));
            default:
                return std::unique_ptr<string_column>(new string_column(info.element_size));
        }
    }

    pybind11::list as_python_list(std::vector<std::unique_ptr<masked_column>> & objects)
    {
        pybind11::list result;
        for (auto & object : objects) {
            result.append(pybind11::make_tuple(object->get_data(), object->get_mask()));
        }
        return result;
    }
}

numpy_result_set::numpy_result_set(turbodbc::result_sets::result_set & base) :
    base_result_(base)
{
}

pybind11::object numpy_result_set::fetch_next_batch()
{
    std::size_t rows_in_batch = 0;
    {
        pybind11::gil_scoped_release release;
        rows_in_batch = base_result_.fetch_next_batch();
    }
    auto const column_info = base_result_.get_column_info();
    auto const n_columns = column_info.size();

    std::vector<std::unique_ptr<masked_column>> columns;
    for (std::size_t i = 0; i != n_columns; ++i) {
        columns.push_back(make_masked_column(column_info[i]));
    }

    auto const buffers = base_result_.get_buffers();

    for (std::size_t i = 0; i != n_columns; ++i) {
        columns[i]->append(buffers[i].get(), rows_in_batch);
    }

    return as_python_list(columns);
}



}
