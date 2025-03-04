#pragma once

#include "cpp_odbc/statement.h"
#include "gmock/gmock.h"

namespace cpp_odbc_test {

class mock_statement : public cpp_odbc::statement {
public:
	MOCK_CONST_METHOD1( do_get_integer_attribute, intptr_t(SQLINTEGER));
	MOCK_CONST_METHOD2( do_set_attribute, void(SQLINTEGER, intptr_t));
	MOCK_CONST_METHOD2( do_set_attribute, void(SQLINTEGER, SQLULEN *));
	MOCK_CONST_METHOD1( do_execute, void(std::string const &));
	MOCK_CONST_METHOD1( do_prepare, void(std::string const &));
	MOCK_CONST_METHOD1( do_prepare, void(std::u16string const &));
	MOCK_CONST_METHOD5( do_bind_input_parameter, void(SQLUSMALLINT, SQLSMALLINT, SQLSMALLINT, SQLSMALLINT, cpp_odbc::multi_value_buffer &));
	MOCK_CONST_METHOD0( do_unbind_all_parameters, void());
	MOCK_CONST_METHOD0( do_execute_prepared, void());
	MOCK_CONST_METHOD0( do_number_of_columns, short int());
	MOCK_CONST_METHOD0( do_number_of_parameters, short int());
	MOCK_CONST_METHOD3( do_bind_column, void(SQLUSMALLINT, SQLSMALLINT, cpp_odbc::multi_value_buffer &));
	MOCK_CONST_METHOD0( do_unbind_all_columns, void());
	MOCK_CONST_METHOD0( do_fetch_next, bool());
	MOCK_CONST_METHOD0( do_close_cursor, void());
	MOCK_CONST_METHOD2( do_get_integer_column_attribute, intptr_t(SQLUSMALLINT, SQLUSMALLINT));
	MOCK_CONST_METHOD2( do_get_string_column_attribute, std::string(SQLUSMALLINT, SQLUSMALLINT));
	MOCK_CONST_METHOD0( do_row_count, SQLLEN());
	MOCK_CONST_METHOD1( do_describe_column, cpp_odbc::column_description(SQLUSMALLINT));
	MOCK_CONST_METHOD1( do_describe_column_wide, cpp_odbc::column_description(SQLUSMALLINT));
	MOCK_CONST_METHOD1( do_describe_parameter, cpp_odbc::column_description(SQLUSMALLINT));
	MOCK_CONST_METHOD0( do_more_results, bool());
	MOCK_METHOD0( do_finalize, void());
};

}
