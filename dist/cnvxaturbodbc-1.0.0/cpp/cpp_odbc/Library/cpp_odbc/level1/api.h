#pragma once

#ifdef _WIN32
#include <windows.h>
#endif
#include "sql.h"

namespace cpp_odbc { namespace level1 {

/**
 * @brief This class serves as an interface to the unixodbc API. It has the same low-level signatures as the original
 *        unixodbc API. No high-level concepts have been introduced. Since no original functions are introduced,
 *        documentation for all functions including parameters can be found in unixodbc's documentation.
 */
class api {
public:
    virtual ~api();

    api(api const &) = delete;
    api & operator=(api const &) = delete;

    /**
     * @brief see unixodbc's SQLAllocHandle() function.
     */
    SQLRETURN allocate_handle(SQLSMALLINT handle_type, SQLHANDLE input_handle, SQLHANDLE * output_handle_ptr) const;

    /**
     * @brief see unixodbc's SQLFreeHandle() function.
     */
    SQLRETURN free_handle(SQLSMALLINT handle_type, SQLHANDLE handle) const;

    /**
     * @brief see unixodbc's SQLGetDiagRec() function.
     */
    SQLRETURN get_diagnostic_record(SQLSMALLINT handle_type, SQLHANDLE handle, SQLSMALLINT record_id, SQLCHAR * status_code_ptr, SQLINTEGER * native_error_ptr, SQLCHAR * message_text, SQLSMALLINT buffer_length, SQLSMALLINT * text_length_ptr) const;

    /**
     * @brief see unixodbc's SQLSetEnvAttr() function
     */
    SQLRETURN set_environment_attribute(SQLHENV environment_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const;

    /**
     * @brief see unixodbc's SQLSetConnectAttr() function
     */
    SQLRETURN set_connection_attribute(SQLHDBC connection_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const;

    /**
     * @brief see unixodbc's SQLDriverConnect() function
     */
    SQLRETURN establish_connection(SQLHDBC connection_handle, SQLHWND window_handle, SQLCHAR * input_connection_string, SQLSMALLINT input_connection_string_length, SQLCHAR * out_connection_string, SQLSMALLINT output_connection_string_buffer_length, SQLSMALLINT * output_connection_string_length, SQLUSMALLINT driver_completion) const;

    /**
     * @brief see unixodbc's SQLDisconnect() function
     */
    SQLRETURN disconnect(SQLHDBC connection_handle) const;

    /**
     * @brief see unixodbc's SQLEndTran() function
     */
    SQLRETURN end_transaction(SQLSMALLINT handle_type, SQLHANDLE connection_or_environment_handle, SQLSMALLINT completion_type) const;

    /**
     * @brief see unixodbc's SQLGetInfo() function
     */
    SQLRETURN get_connection_info(SQLHDBC connection_handle, SQLUSMALLINT info_type, SQLPOINTER info_value_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr) const;

    /**
     * @brief see unixodbc's SQLBindCol() function
     */
    SQLRETURN bind_column(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLSMALLINT target_type, SQLPOINTER target_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const;

    /**
     * @brief see unixodbc's SQLBindParameter() function
     */
    SQLRETURN bind_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_id, SQLSMALLINT input_output_type, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLULEN column_size, SQLSMALLINT decimal_digits, SQLPOINTER parameter_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const;

    /**
     * @brief see unixodbc's SQLColAttribute() function
     */
    SQLRETURN column_attribute(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier, SQLPOINTER character_attribute_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr, SQLLEN * numeric_attribute_ptr) const;

    /**
     * @brief see unixodbc's SQLExecute() function
     */
    SQLRETURN execute_prepared_statement(SQLHSTMT statement_handle) const;

    /**
     * @brief see unixodbc's SQLExecDirect() function
     */
    SQLRETURN execute_statement(SQLHSTMT statement_handle, SQLCHAR * sql_text, SQLINTEGER sql_length) const;

    /**
     * @brief see unixodbc's SQLFetchScroll() function
     */
    SQLRETURN fetch_scroll(SQLHSTMT statement_handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const;

    /**
     * @brief see unixodbc's SQLFreeStmt() function
     */
    SQLRETURN free_statement(SQLHSTMT statement_handle, SQLUSMALLINT option) const;

    /**
     * @brief see unixodbc's SQLGetStmtAttr() function
     */
    SQLRETURN get_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER buffer_length, SQLINTEGER * string_length_ptr) const;

    /**
     * @brief see unixodbc's SQLNumResultCols() function
     */
    SQLRETURN number_of_result_columns(SQLHSTMT statement_handle, SQLSMALLINT * destination) const;

    /**
     * @brief see unixodbc's SQLNumParams() function
     */
    SQLRETURN number_of_parameters(SQLHSTMT statement_handle, SQLSMALLINT * destination) const;

    /**
     * @brief see unixodbc's SQLPrepare() function
     */
    SQLRETURN prepare_statement(SQLHSTMT statement_handle, SQLCHAR * statement_text, SQLINTEGER text_length) const;

    /**
     * @brief see unixodbc's SQLPrepareW() function
     */
    SQLRETURN prepare_statement(SQLHSTMT statement_handle, SQLWCHAR * statement_text, SQLINTEGER text_length) const;

    /**
     * @brief see unixodbc's SQLSetStmtAttr() function
     */
    SQLRETURN set_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const;

    /**
     * @brief see unixodbc's SQLRowCount() function
     */
    SQLRETURN row_count(SQLHSTMT statement_handle, SQLLEN * count) const;

    /**
     * @brief see unixodbc's SQLDescribeCol() function
     */
    SQLRETURN describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const;

    /**
     * @brief see unixodbc's SQLDescribeColW() function
     */
    SQLRETURN describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLWCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const;

    /**
     * @brief see unixodbc's SQLDescribeParam() function
     */
    SQLRETURN describe_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_number, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const;

    /**
     * @brief see unixodbc's SQLMoreResults() function
     */
    SQLRETURN more_results(SQLHSTMT statement_handle) const;

    /**
     * @brief see unixodbc's SQLGetFunctions() function
     */
    SQLRETURN get_functions(SQLHDBC connection_handle, SQLUSMALLINT function_id, SQLUSMALLINT * is_supported) const;

protected:

    api();

private:

    virtual SQLRETURN do_allocate_handle(SQLSMALLINT handle_type, SQLHANDLE input_handle, SQLHANDLE * output_handle_ptr) const = 0;
    virtual SQLRETURN do_free_handle(SQLSMALLINT handle_type, SQLHANDLE handle) const = 0;
    virtual SQLRETURN do_get_diagnostic_record(SQLSMALLINT handle_type, SQLHANDLE handle, SQLSMALLINT record_id, SQLCHAR * status_code_ptr, SQLINTEGER * native_error_ptr, SQLCHAR * message_text, SQLSMALLINT buffer_length, SQLSMALLINT * text_length_ptr) const = 0;
    virtual SQLRETURN do_set_environment_attribute(SQLHENV environment_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const = 0;
    virtual SQLRETURN do_set_connection_attribute(SQLHDBC connection_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const = 0;
    virtual SQLRETURN do_establish_connection(SQLHDBC connection_handle, SQLHWND window_handle, SQLCHAR * input_connection_string, SQLSMALLINT input_connection_string_length, SQLCHAR * out_connection_string, SQLSMALLINT output_connection_string_buffer_length, SQLSMALLINT * output_connection_string_length, SQLUSMALLINT driver_completion) const = 0;
    virtual SQLRETURN do_disconnect(SQLHDBC connection_handle) const = 0;
    virtual SQLRETURN do_end_transaction(SQLSMALLINT handle_type, SQLHANDLE connection_or_environment_handle, SQLSMALLINT completion_type) const = 0;
    virtual SQLRETURN do_get_connection_info(SQLHDBC connection_handle, SQLUSMALLINT info_type, SQLPOINTER info_value_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr) const = 0;

    virtual SQLRETURN do_bind_column(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLSMALLINT target_type, SQLPOINTER target_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const = 0;
    virtual SQLRETURN do_bind_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_id, SQLSMALLINT input_output_type, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLULEN column_size, SQLSMALLINT decimal_digits, SQLPOINTER parameter_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const = 0;
    virtual SQLRETURN do_column_attribute(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier, SQLPOINTER character_attribute_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr, SQLLEN * numeric_attribute_ptr) const = 0;
    virtual SQLRETURN do_execute_prepared_statement(SQLHSTMT statement_handle) const = 0;
    virtual SQLRETURN do_execute_statement(SQLHSTMT statement_handle, SQLCHAR * sql_text, SQLINTEGER sql_length) const = 0;
    virtual SQLRETURN do_fetch_scroll(SQLHSTMT statement_handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const = 0;
    virtual SQLRETURN do_free_statement(SQLHSTMT statement_handle, SQLUSMALLINT option) const = 0;
    virtual SQLRETURN do_get_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER buffer_length, SQLINTEGER * string_length_ptr) const = 0;
    virtual SQLRETURN do_number_of_result_columns(SQLHSTMT statement_handle, SQLSMALLINT * destination) const = 0;
    virtual SQLRETURN do_number_of_parameters(SQLHSTMT statement_handle, SQLSMALLINT * destination) const = 0;
    virtual SQLRETURN do_prepare_statement(SQLHSTMT statement_handle, SQLCHAR * statement_text, SQLINTEGER text_length) const = 0;
    virtual SQLRETURN do_prepare_statement(SQLHSTMT statement_handle, SQLWCHAR * statement_text, SQLINTEGER text_length) const = 0;
    virtual SQLRETURN do_set_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const = 0;
    virtual SQLRETURN do_row_count(SQLHSTMT statement_handle, SQLLEN * count) const = 0;
    virtual SQLRETURN do_describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const = 0;
    virtual SQLRETURN do_describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLWCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const = 0;
    virtual SQLRETURN do_describe_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_number, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const = 0;
    virtual SQLRETURN do_more_results(SQLHSTMT statement_handle) const = 0;
    virtual SQLRETURN do_get_functions(SQLHDBC connection_handle, SQLUSMALLINT function_id, SQLUSMALLINT * is_supported) const = 0;
};


} }
