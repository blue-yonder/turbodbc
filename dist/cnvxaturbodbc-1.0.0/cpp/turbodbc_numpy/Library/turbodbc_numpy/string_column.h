#pragma once

#include <turbodbc_numpy/masked_column.h>
#include <turbodbc/type_code.h>


namespace turbodbc_numpy {

class string_column : public masked_column {
public:
    string_column(std::size_t total_buffer_size);
    virtual ~string_column();

private:
    void do_append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values) final;

    pybind11::object do_get_data() final;
    pybind11::object do_get_mask() final;

    std::size_t maximum_text_size_;
    pybind11::list data_;
};

}
