#include <turbodbc_numpy/numpy_type.h>

#include <turbodbc_numpy/ndarrayobject.h>

namespace turbodbc_numpy {

numpy_type const numpy_int_type = {NPY_INT64, 8};
numpy_type const numpy_double_type = {NPY_FLOAT64, 8};
numpy_type const numpy_bool_type = {NPY_BOOL, 1};
numpy_type const numpy_datetime_type = {NPY_DATETIME, 8};

}
