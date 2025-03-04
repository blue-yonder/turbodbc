#pragma once

#include "cpp_odbc/level3/raii_connection.h"
#include "cpp_odbc/level3/raii_environment.h"
#include "cpp_odbc/level3/raii_statement.h"

namespace cpp_odbc {

/**
 * @brief This namespace contains implementations of the interfaces present in the cpp_odbc
 *        namespace which do resource management in proper C++ style following the RAII
 *        (resource acquisition is initialization) idiom.
 *        Users should not instantiate these classes directly, but should rather use
 *        cpp_odbc::make_environment() and start from there.
 */
namespace level3 {}


}
