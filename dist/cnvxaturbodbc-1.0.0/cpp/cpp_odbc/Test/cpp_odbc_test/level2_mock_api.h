#pragma once

#include "cpp_odbc/level2/api.h"

#include "gmock/gmock.h"

namespace cpp_odbc_test {

	struct level2_mock_api : public cpp_odbc::level2::api {
		level2_mock_api();
		virtual ~level2_mock_api();
		MOCK_CONST_METHOD1(do_allocate_statement_handle, cpp_odbc::level2::statement_handle(cpp_odbc::level2::connection_handle const &));
		MOCK_CONST_METHOD1(do_allocate_connection_handle, cpp_odbc::level2::connection_handle(cpp_odbc::level2::environment_handle const &));
		MOCK_CONST_METHOD0(do_allocate_environment_handle, cpp_odbc::level2::environment_handle());
		MOCK_CONST_METHOD1(do_free_handle, void(cpp_odbc::level2::statement_handle &));
		MOCK_CONST_METHOD1(do_free_handle, void(cpp_odbc::level2::connection_handle &));
		MOCK_CONST_METHOD1(do_free_handle, void(cpp_odbc::level2::environment_handle &));
		MOCK_CONST_METHOD1(do_get_diagnostic_record, cpp_odbc::level2::diagnostic_record(cpp_odbc::level2::statement_handle const &));
		MOCK_CONST_METHOD1(do_get_diagnostic_record, cpp_odbc::level2::diagnostic_record(cpp_odbc::level2::connection_handle const &));
		MOCK_CONST_METHOD1(do_get_diagnostic_record, cpp_odbc::level2::diagnostic_record(cpp_odbc::level2::environment_handle const &));
		MOCK_CONST_METHOD3(do_set_environment_attribute, void(cpp_odbc::level2::environment_handle const &, SQLINTEGER, intptr_t));
		MOCK_CONST_METHOD3(do_set_connection_attribute, void(cpp_odbc::level2::connection_handle const &, SQLINTEGER, intptr_t));
		MOCK_CONST_METHOD2(do_establish_connection, void(cpp_odbc::level2::connection_handle &, std::string const &));
		MOCK_CONST_METHOD1(do_disconnect, void(cpp_odbc::level2::connection_handle &));
		MOCK_CONST_METHOD2(do_end_transaction, void(cpp_odbc::level2::connection_handle const &, SQLSMALLINT));
		MOCK_CONST_METHOD2(do_get_string_connection_info, std::string(cpp_odbc::level2::connection_handle const & handle, SQLUSMALLINT info_type));
		MOCK_CONST_METHOD2(do_get_integer_connection_info, SQLUINTEGER(cpp_odbc::level2::connection_handle const & handle, SQLUSMALLINT info_type));
		MOCK_CONST_METHOD4(do_bind_column, void(cpp_odbc::level2::statement_handle const &, SQLUSMALLINT, SQLSMALLINT, cpp_odbc::multi_value_buffer &));
		MOCK_CONST_METHOD6(do_bind_input_parameter, void(cpp_odbc::level2::statement_handle const &, SQLUSMALLINT, SQLSMALLINT, SQLSMALLINT, SQLSMALLINT, cpp_odbc::multi_value_buffer &));
		MOCK_CONST_METHOD1(do_execute_prepared_statement, void(cpp_odbc::level2::statement_handle const &));
		MOCK_CONST_METHOD2(do_execute_statement, void(cpp_odbc::level2::statement_handle const &, std::string const &));
		MOCK_CONST_METHOD3(do_fetch_scroll, bool(cpp_odbc::level2::statement_handle const &, SQLSMALLINT, SQLLEN));
		MOCK_CONST_METHOD2(do_free_statement, void(cpp_odbc::level2::statement_handle const &, SQLUSMALLINT));
		MOCK_CONST_METHOD3(do_get_integer_column_attribute, intptr_t(cpp_odbc::level2::statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier));
		MOCK_CONST_METHOD2(do_get_integer_statement_attribute, intptr_t(cpp_odbc::level2::statement_handle const &, SQLINTEGER));
		MOCK_CONST_METHOD3(do_get_string_column_attribute, std::string(cpp_odbc::level2::statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier));
		MOCK_CONST_METHOD1(do_number_of_result_columns, short int(cpp_odbc::level2::statement_handle const &));
		MOCK_CONST_METHOD1(do_number_of_parameters, short int(cpp_odbc::level2::statement_handle const &));
		MOCK_CONST_METHOD2(do_prepare_statement, void(cpp_odbc::level2::statement_handle const &, std::string const &));
		MOCK_CONST_METHOD2(do_prepare_statement, void(cpp_odbc::level2::statement_handle const &, std::u16string const &));
		MOCK_CONST_METHOD3(do_set_statement_attribute, void(cpp_odbc::level2::statement_handle const &, SQLINTEGER, intptr_t));
		MOCK_CONST_METHOD3(do_set_statement_attribute, void(cpp_odbc::level2::statement_handle const &, SQLINTEGER, SQLULEN *));
		MOCK_CONST_METHOD1(do_row_count, SQLLEN(cpp_odbc::level2::statement_handle const &));
		MOCK_CONST_METHOD2(do_describe_column, cpp_odbc::column_description(cpp_odbc::level2::statement_handle const &, SQLUSMALLINT));
		MOCK_CONST_METHOD2(do_describe_column_wide, cpp_odbc::column_description(cpp_odbc::level2::statement_handle const &, SQLUSMALLINT));
		MOCK_CONST_METHOD2(do_describe_parameter, cpp_odbc::column_description(cpp_odbc::level2::statement_handle const &, SQLUSMALLINT));
		MOCK_CONST_METHOD1(do_more_results, bool(cpp_odbc::level2::statement_handle const &));
		MOCK_CONST_METHOD2(do_supports_function, bool(cpp_odbc::level2::connection_handle const &, SQLUSMALLINT));
	};

}
