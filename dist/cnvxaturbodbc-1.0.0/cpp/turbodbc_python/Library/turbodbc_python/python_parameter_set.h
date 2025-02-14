#pragma once

#include <turbodbc/parameter_sets/bound_parameter_set.h>
#include <vector>
#include <pybind11/pybind11.h>

namespace turbodbc {

class python_parameter_set {
public:
	python_parameter_set(bound_parameter_set & parameters);

	void add_parameter_set(pybind11::iterable const & parameter_set);

	void flush();

	~python_parameter_set();

private:
	void check_parameter_set(pybind11::iterable const & parameter_set) const;
	void add_parameter(std::size_t index, pybind11::handle const & value);
	void recover_unwritten_parameters_below(std::size_t parameter_index, std::size_t last_active_row);

	bound_parameter_set & parameters_;
	std::size_t current_parameter_set_;
};

}
