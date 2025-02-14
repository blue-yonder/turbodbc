#pragma once

#include <turbodbc/parameter_sets/bound_parameter_set.h>

#include <pybind11/numpy.h>

namespace turbodbc_numpy {

void set_numpy_parameters(turbodbc::bound_parameter_set & parameters, std::vector<std::tuple<pybind11::array, pybind11::array_t<bool>, std::string>> const & columns);

}
