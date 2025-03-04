#pragma once

#include "cpp_odbc/level2/api.h"
#include "cpp_odbc/level2/diagnostic_record.h"
#include "cpp_odbc/level2/fixed_length_string_buffer.h"
#include "cpp_odbc/level2/handles.h"
#include "cpp_odbc/level2/input_string_buffer.h"
#include "cpp_odbc/level2/level1_connector.h"
#include "cpp_odbc/level2/string_buffer.h"

namespace cpp_odbc {

/**
 * @brief This namespace contains C++ified versions of the interface defined in cpp_odbc::level1.
 *        It contains all necessary abstractions for handling single-value data buffers
 *        and for dealing with exceptions.
 *        Features of this namespace are required in here or are intended to be used by classes in
 *        cpp_odbc::level3.
 */
namespace level2 {}


}
