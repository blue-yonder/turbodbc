#pragma once

#include <turbodbc_numpy/masked_column.h>
#include <turbodbc_numpy/numpy_type.h>

namespace turbodbc_numpy {

class binary_column : public masked_column {
public:
	binary_column(numpy_type const & type);
	virtual ~binary_column();
private:
	void do_append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values) final;

	pybind11::object do_get_data() final;
	pybind11::object do_get_mask() final;

	void resize(std::size_t new_size);

	numpy_type type_;
	pybind11::object data_;
	pybind11::object mask_;
	std::size_t size_;
};

}
