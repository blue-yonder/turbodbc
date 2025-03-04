#pragma once

namespace turbodbc_numpy {

struct numpy_type {
	int code;
	int size;
};

extern numpy_type const numpy_int_type;
extern numpy_type const numpy_double_type;
extern numpy_type const numpy_bool_type;
extern numpy_type const numpy_datetime_type;

}
