#pragma once

#include <turbodbc_numpy/masked_column.h>
#include <turbodbc/type_code.h>

#include <functional>

namespace turbodbc_numpy {

class datetime_column : public masked_column {
public:
	datetime_column(turbodbc::type_code type);
	virtual ~datetime_column();

	typedef std::function<int64_t (char const * data_pointer)> converter;
private:
	void do_append(cpp_odbc::multi_value_buffer const & buffer, std::size_t n_values) final;

	pybind11::object do_get_data() final;
	pybind11::object do_get_mask() final;

	void resize(std::size_t new_size);

	turbodbc::type_code type_;
	pybind11::object data_;
	pybind11::object mask_;
	std::size_t size_;
	converter converter_;
};

}
