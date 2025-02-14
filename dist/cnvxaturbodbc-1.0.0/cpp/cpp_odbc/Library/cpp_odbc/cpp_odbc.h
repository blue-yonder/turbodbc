#pragma once
/**
 *  @file cpp_odbc.h
 *  @date 05.12.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate$
 *  $LastChangedBy$
 *  $LastChangedRevision$
 *
 */

#include "cpp_odbc/connection.h"
#include "cpp_odbc/credentials.h"
#include "cpp_odbc/environment.h"
#include "cpp_odbc/error.h"
#include "cpp_odbc/make_environment.h"
#include "cpp_odbc/multi_value_buffer.h"
#include "cpp_odbc/statement.h"

/**
 * @brief This namespace contains all classes and functions which belong to the
 *        cpp_odbc library.
 *
 *        The main namespace only contains high-level features intended for
 *        library users. A good starting point is to call cpp_odbc::make_environment()
 *        and descend from there.
 *
 *        The classes found here do not form a complete database driver, but should enable
 *        users to build one themselves quickly.
 */
namespace cpp_odbc {}
