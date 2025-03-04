#pragma once
/*
 * mock_classes.h
 *
 *  Created on: 19.12.2014
 *      Author: mwarsinsky
 */


#include "gmock/gmock.h"

#include "cpp_odbc/connection.h"
#include "turbodbc/column.h"

namespace turbodbc_test {

	class default_mock_connection : public cpp_odbc::connection {
	public:
		default_mock_connection();
		~default_mock_connection();
		MOCK_CONST_METHOD0( do_make_statement, std::shared_ptr<cpp_odbc::statement const>());
		MOCK_CONST_METHOD2( do_set_attribute, void(SQLINTEGER, intptr_t));
		MOCK_CONST_METHOD0( do_commit, void());
		MOCK_CONST_METHOD0( do_rollback, void());
		MOCK_CONST_METHOD1( do_get_string_info, std::string(SQLUSMALLINT));
		MOCK_CONST_METHOD1( do_get_integer_info, SQLUINTEGER(SQLUSMALLINT));
		MOCK_CONST_METHOD1( do_supports_function, bool(SQLUSMALLINT));
	};


	class default_mock_statement : public cpp_odbc::statement {
	public:
		default_mock_statement();
		~default_mock_statement();
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

    using mock_connection = testing::NiceMock<default_mock_connection>;
    using mock_statement = testing::NiceMock<default_mock_statement>;


}



