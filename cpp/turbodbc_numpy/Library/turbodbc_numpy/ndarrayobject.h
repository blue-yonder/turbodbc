#pragma once

/**
 * Include this file instead of the original Numpy header so common
 * problems with the API can be solved at a central position.
 */

// compare http://docs.scipy.org/doc/numpy/reference/c-api.array.html#importing-the-api
// as to why these defines are necessary
#define PY_ARRAY_UNIQUE_SYMBOL turbodbc_numpy_API
#define NO_IMPORT_ARRAY
#include <Python.h>
#include <numpy/ndarrayobject.h>
