#pragma once

#include <turbodbc_numpy/masked_column.h>
#include <turbodbc/type_code.h>


namespace turbodbc_numpy {

class unicode_column : public masked_column {
public:
	unicode_column();
	virtual ~unicode_column();

private:
	void do_append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values) final;

	pybind11::object do_get_data() final;
	pybind11::object do_get_mask() final;

	pybind11::list data_;
};

}
