#pragma once
/**
 *  @file diagnostic_record.h
 *  @date 03.03.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate: 2014-11-28 11:59:59 +0100 (Fr, 28 Nov 2014) $
 *  $LastChangedBy: mkoenig $
 *  $LastChangedRevision: 21206 $
 *
 */

#include <string>

namespace cpp_odbc { namespace level2 {

/**
 * @brief This struct represents diagnostics information from a unixodbc driver
 */
struct diagnostic_record {
	std::string odbc_status_code;	///< status code as given by unixodbc
	int native_error_code;			///< error code as given by the ODBC driver
	std::string message;			///< error message as given by the ODBC driver
};

} }
