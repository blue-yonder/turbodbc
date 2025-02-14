#pragma once

#include <turbodbc_numpy/numpy_type.h>

#include <pybind11/pybind11.h>

namespace turbodbc_numpy {

/**
 * @brief Create new, empty numpy array based on numpy type constants or a
 *        type's string representation
 */
pybind11::object make_empty_numpy_array(numpy_type const & type);
pybind11::object make_empty_numpy_array(std::string const & descriptor);

}
