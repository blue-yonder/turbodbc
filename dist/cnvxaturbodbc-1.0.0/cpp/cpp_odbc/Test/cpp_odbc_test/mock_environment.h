#pragma once
/**
 *  @file mock_environment.h
 *  @date 05.12.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate$
 *  $LastChangedBy$
 *  $LastChangedRevision$
 *
 */

#include "cpp_odbc/environment.h"

#include "gmock/gmock.h"

namespace cpp_odbc_test {

	class mock_environment : public cpp_odbc::environment {
	public:
		MOCK_CONST_METHOD1(do_make_connection, std::shared_ptr<cpp_odbc::connection>(std::string const &));
		MOCK_CONST_METHOD2(do_set_attribute, void(SQLINTEGER, intptr_t));
	};

}
