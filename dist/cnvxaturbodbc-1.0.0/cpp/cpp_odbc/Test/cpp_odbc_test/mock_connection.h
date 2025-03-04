#pragma once
/**
 *  @file mock_connection.h
 *  @date 16.05.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate: 2014-11-28 15:26:51 +0100 (Fr, 28 Nov 2014) $
 *  $LastChangedBy: mkoenig $
 *  $LastChangedRevision: 21210 $
 *
 */

#include "cpp_odbc/connection.h"

#include "gmock/gmock.h"

namespace cpp_odbc_test {

	class mock_connection : public cpp_odbc::connection {
	public:
		MOCK_CONST_METHOD0( do_make_statement, std::shared_ptr<cpp_odbc::statement const>());
		MOCK_CONST_METHOD2( do_set_attribute, void(SQLINTEGER, intptr_t));
		MOCK_CONST_METHOD0( do_commit, void());
		MOCK_CONST_METHOD0( do_rollback, void());
		MOCK_CONST_METHOD1( do_get_string_info, std::string(SQLUSMALLINT info_type));
		MOCK_CONST_METHOD1( do_get_integer_info, SQLUINTEGER(SQLUSMALLINT info_type));
		MOCK_CONST_METHOD1( do_supports_function, bool(SQLUSMALLINT));
	};

}
