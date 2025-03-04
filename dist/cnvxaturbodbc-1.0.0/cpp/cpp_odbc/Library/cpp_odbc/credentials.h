#pragma once
/**
 *  @file credentials.h
 *  @date Sep 3, 2013
 *  @author mkoenig
 *  @brief
 *
 *  $LastChangedDate: 2014-12-05 08:55:14 +0100 (Fr, 05 Dez 2014) $
 *  $LastChangedBy: mkoenig $
 *  $LastChangedRevision: 21240 $
 *
 */

#include <string>

namespace cpp_odbc {

/**
 * @brief A struct which contains user name and password for use to authenticate with a database
 */
struct credentials {
	std::string user;		///< database user name
	std::string password;	///< password associated with user
};

}
