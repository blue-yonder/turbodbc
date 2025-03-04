#pragma once

#ifdef _WIN32
#include <windows.h>
#endif
#include "sql.h"
#include "cpp_odbc/level2/diagnostic_record.h"
#include "cpp_odbc/level2/handles.h"
#include "cpp_odbc/multi_value_buffer.h"
#include "cpp_odbc/column_description.h"

namespace cpp_odbc { namespace level2 {

/**
 * @brief This class serves as a higher-level interface to unixODBC's functionalities.
 *        All functions from unixodbc_api have been reworked. Expect the following improvements
 *        from this interface:
 *
 *        - generic handle types and void pointers have been replaced by actual types
 *        - where functions could do more than one thing, e.g., reading either strings
 *          or integers, they have been split into multiple ones.
 *        - pointers to buffers, size variables, and length counters have been combined
 *          to real buffer types
 *        - output parameters have been replaced by return values
 *        - return codes have been eliminated. Implementations should use exceptions
 *          to indicate failures.
 *
 *        Most importantly, this interface is a reasonable building block for constructing
 *        still more high-level functionality such as RAII classes, result sets, etc.
 *        Using this interface should feel like dealing with a C++ API instead of a C one.
 */
class api {
public:
    virtual ~api();

    api(api const &) = delete;
    api & operator=(api const &) = delete;

    /**
     * @brief Create a new statement handle.
     * @param input_handle The statement handle is associated with this connection handle
     * @return A new statement handle.
     */
    statement_handle allocate_statement_handle(connection_handle const & input_handle) const;

    /**
     * @brief Create a new connection handle
     * @param input_handle The statement handle is associated with this environment handle
     * @return A new connection handle
     */
    connection_handle allocate_connection_handle(environment_handle const & input_handle) const;

    /**
     * @brief Create a new environment handle
     * @return A new environment handle
     */
    environment_handle allocate_environment_handle() const;

    /**
     * @brief Free the given statement handle
     * @param handle The handle to free
     */
    void free_handle(statement_handle & handle) const;

    /**
     * @brief Free the given connection handle
     * @param handle The handle to free
     */
    void free_handle(connection_handle & handle) const;

    /**
     * @brief Free the given environment handle
     * @param handle The handle to free
     */
    void free_handle(environment_handle & handle) const;

    /**
     * @brief Retrieve error diagnostics associated with the given handle
     * @param handle Only diagnostic records associated with this handle are retrieved
     * @return Error diagnostics (first record unixODBC offers)
     */
    diagnostic_record get_diagnostic_record(statement_handle const & handle) const;

    /**
     * @brief Retrieve error diagnostics associated with the given handle
     * @param handle Only diagnostic records associated with this handle are retrieved
     * @return Error diagnostics (first record unixODBC offers)
     */
    diagnostic_record get_diagnostic_record(connection_handle const & handle) const;

    /**
     * @brief Retrieve error diagnostics associated with the given handle
     * @param handle Only diagnostic records associated with this handle are retrieved
     * @return Error diagnostics (first record unixODBC offers)
     */
    diagnostic_record get_diagnostic_record(environment_handle const & handle) const;

    /**
     * @brief Set an attribute of the environment to a specific integer value
     * @param handle Set the attribute for the environment associated with this handle
     * @param attribute Set this attribute. Constants should be employed according to
     *        the documentation of unixODBC's SQLSetEnvAttr() function
     * @param value The new integer value of the attribute.
     */
    void set_environment_attribute(environment_handle const & handle, SQLINTEGER attribute, intptr_t value) const;

    /**
     * @brief Set an attribute of the connection to a specific integer value
     * @param handle Set the attribute for the connection associated with this handle
     * @param attribute Set this attribute. Constants should be employed according to
     *        the documentation of unixODBC's SQLSetConnectAttr() function
     * @param value The new integer value of the attribute.
     */
    void set_connection_attribute(connection_handle const & handle, SQLINTEGER attribute, intptr_t value) const;

    /**
     * @brief Establish a connection to a database via the given handle
     * @param handle The connection will be established for this handle
     * @param connection_string This is the connection string which described the target database
     */
    void establish_connection(connection_handle & handle, std::string const & connection_string) const;

    /**
     * @brief Disconnect the connection associated with the connection handle
     * @param handle The connection associated with this handle will be closed.
     */
    void disconnect(connection_handle & handle) const;

    /**
     * @brief End a transaction on the current connection with the command given by completion_type
     * @param handle The connection which houses the still active transaction
     * @param completion_type Use this command. See the documentation of unixODBC's
     *        SQLEndTran() function.
     */
    void end_transaction(connection_handle const & handle, SQLSMALLINT completion_type) const;

    /**
     * @brief Retrieve a string information associated with the connection
     * @param handle The connection for which we would like to retrieve information
     * @param info_type The info type we are interested in. See unixODBC's SQLGetInfo() documentation
     * @return The retrieved information string
     */
    std::string get_string_connection_info(connection_handle const & handle, SQLUSMALLINT info_type) const;

    /**
     * @brief Retrieve an integer information associated with the connection
     * @param handle The connection for which we would like to retrieve information
     * @param info_type The info type we are interested in. See unixODBC's SQLGetInfo() documentation
     * @return The retrieved integer
     */
    SQLUINTEGER get_integer_connection_info(connection_handle const & handle, SQLUSMALLINT info_type) const;

    /**
     * @brief Bind a buffer to a column of the statement
     * @param handle The statement which holds a result set
     * @param column_id The column identifier with which we want to associate the buffer
     * @param column_type The identifier of the C data type of the column. See unixODBC's SQLBindCol() documentation.
     * @param column_buffer A buffer which will be filled with data whenever fetch_scroll() is called.
     */
    void bind_column(statement_handle const & handle, SQLUSMALLINT column_id, SQLSMALLINT column_type, multi_value_buffer & column_buffer) const;

    /**
     * @brief Bind a buffer for use as an input parameter
     * @param handle The parameter shall be bound to this statement
     * @param parameter_id The parameter identifier
     * @param value_type The C data type identifier of the buffer. See unixODBC's SQLBindParameter() documentation
     * @param parameter_type The SQL data type identifier of the buffer. See unixODBC's SQLBindParameter() documentation
     * @param parameter_values The buffer which shall be bound as a parameter.
     */
    void bind_input_parameter(statement_handle const & handle, SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, multi_value_buffer & parameter_values) const;

    /**
     * @brief Executes an SQL query which has previously been prepared
     * @param handle The statement handle which shall be prepared
     */
    void execute_prepared_statement(statement_handle const & handle) const;

    /**
     * @brief Executes an SQL query
     * @param handle The statement handle which shall be prepared
     * @param sql The SQL query
     */
    void execute_statement(statement_handle const & handle, std::string const & sql) const;

    /**
     * @brief Fetch additional data from the result of the previously executed statement and
     *        store it in previously bound buffers
     * @param handle The statement which holds the result set
     * @param fetch_orientation Determines the positioning of the result cursor.
     *        See unixODBC's SQLFetchScroll() documentation.
     * @param fetch_offset Determines which row to fetch depending on the cursor positioning.
     *        See unixODBC's SQLFetchScroll() documentation.
     * @return true if fetch retrieved data, else false
     */
    bool fetch_scroll(statement_handle const & statement_handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const;

    /**
     * @brief Stops processing associated with the statement and persues further actions
     *        as defined by the value of option.
     * @param handle The statement which shall be freed
     * @param option The option with which the statement should be freed. See unixODBC's SQLFreeStmt() documentation
     */
    void free_statement(statement_handle const & handle, SQLUSMALLINT option) const;

    /**
     * @brief Retrieve a (intptr_t) integer attribute of a column in the result set
     * @param handle The statement which holds the result set
     * @param column_id The column identifier
     * @param field_identifier Identifies the attribute which shall be retrieved. See unixODBC's SQLColAttribute() documentation
     * @return The value of the attribute for the given column
     */
    intptr_t get_integer_column_attribute(statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const;

    /**
     * @brief Retrieve an integer attribute of a statement
     * @param handle The statement for which a handle shall be received
     * @param attribute The attribute which shall be received. See unixODBC's SQLGetStmtSttr() documentation
     */
    intptr_t get_integer_statement_attribute(statement_handle const & handle, SQLINTEGER attribute) const;

    /**
     * @brief Retrieve a string attribute of a column in the result set
     * @param handle The statement which holds the result set
     * @param column_id The column identifier
     * @param field_identifier Identifies the attribute which shall be retrieved. See unixODBC's SQLColAttribute() documentation
     * @return The value of the attribute for the given column
     */
    std::string get_string_column_attribute(statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const;

    /**
     * @brief Retrieve the number of columns associated with the statement
     * @param handle The statement which holds the result set
     * @return The number of columns in the result set
     */
    short int number_of_result_columns(statement_handle const & handle) const;

    /**
     * @brief Retrieve the number of parameters associated with the statement
     * @param handle The statement which holds the prepared statement
     * @return The number of parameters in the prepared statement
     */
    short int number_of_parameters(statement_handle const & handle) const;

    /**
     * @brief Prepares an SQL query
     * @param handle The statement handle which shall be prepared
     * @param sql The SQL query
     */
    void prepare_statement(statement_handle const & handle, std::string const & sql) const;

    /**
     * @brief Prepares an SQL query
     * @param handle The statement handle which shall be prepared
     * @param sql The SQL query
     */
    void prepare_statement(statement_handle const & handle, std::u16string const & sql) const;

    /**
     * @brief Set an attribute of the statement to a given integer value
     * @param handle Set an attribute for this statement
     * @param attribute Set this attribute. See unixODBC's SQLSetStmtAttr() documentation
     * @param value The value which shall be set for this attribute
     */
    void set_statement_attribute(statement_handle const & handle, SQLINTEGER attribute, intptr_t value) const;

    /**
     * @brief Set an attribute of the statement to a pointer to a SQLULEN value
     * @param handle Set an attribute for this statement
     * @param attribute Set this attribute. See unixODBC's SQLSetStmtAttr() documentation
     * @param pointer The pointer which shall be set for this attribute
     */
    void set_statement_attribute(statement_handle const & handle, SQLINTEGER attribute, SQLULEN * pointer) const;

    /**
     * @brief Determine the number of rows in the result set associated with the statement
     * @param handle The statement which holds the result set
     * @return The number of rows in the result set
     */
    SQLLEN row_count(statement_handle const & handle) const;

    /**
     * @brief Retrieve the description of a column
     * @param handle The statement which holds the result set with the column
     * @param column_id One-based index of the column within the result set
     * @return A description of the column
     */
    column_description describe_column(statement_handle const & handle, SQLUSMALLINT column_id) const;

    /**
     * @brief Retrieve the description of a column using the wide variant of the underlying API
     * @param handle The statement which holds the result set with the column
     * @param column_id One-based index of the column within the result set
     * @return A description of the column
     */
    column_description describe_column_wide(statement_handle const & handle, SQLUSMALLINT column_id) const;

    /**
     * @brief Retrieve the description of a parameter
     * @param handle The statement which holds the prepared statement with the parameter
     * @param parameter_id One-based index of the parameter within the prepared statement
     * @return A description of the parameter
     */
    column_description describe_parameter(statement_handle const & handle, SQLUSMALLINT parameter_id) const;

    /**
     * @brief Set internal data structures to deal with results of next query
     * @param handle The statement handle which shall be prepared
     * @return True if another result set is found
     */
    bool more_results(statement_handle const & handle) const;

    /**
     * @brief Ask the connection whether it supports a certain ODBC function
     * @param handle A handle representing the connection
     * @param function_id The identifier for the ODBC function
     * @return True if the connection supports the function, else false
     */
    bool supports_function(connection_handle const & handle, SQLUSMALLINT function_id) const;

protected:

    api();

private:
    virtual statement_handle do_allocate_statement_handle(connection_handle const & input_handle) const = 0;
    virtual connection_handle do_allocate_connection_handle(environment_handle const & input_handle) const = 0;
    virtual environment_handle do_allocate_environment_handle() const = 0;

    virtual void do_free_handle(statement_handle & handle) const = 0;
    virtual void do_free_handle(connection_handle & handle) const = 0;
    virtual void do_free_handle(environment_handle & handle) const = 0;

    virtual diagnostic_record do_get_diagnostic_record(statement_handle const & handle) const = 0;
    virtual diagnostic_record do_get_diagnostic_record(connection_handle const & handle) const = 0;
    virtual diagnostic_record do_get_diagnostic_record(environment_handle const & handle) const = 0;

    virtual void do_set_environment_attribute(environment_handle const & handle, SQLINTEGER attribute, intptr_t value) const = 0;
    virtual void do_set_connection_attribute(connection_handle const & handle, SQLINTEGER attribute, intptr_t value) const = 0;
    virtual void do_establish_connection(connection_handle & handle, std::string const & connection_string) const = 0;
    virtual void do_disconnect(connection_handle & handle) const = 0;
    virtual void do_end_transaction(connection_handle const & handle, SQLSMALLINT completion_type) const = 0;
    virtual std::string do_get_string_connection_info(connection_handle const & handle, SQLUSMALLINT info_type) const = 0;
    virtual SQLUINTEGER do_get_integer_connection_info(connection_handle const & handle, SQLUSMALLINT info_type) const = 0;

    virtual void do_bind_column(statement_handle const & handle, SQLUSMALLINT column_id, SQLSMALLINT column_type, multi_value_buffer & column_buffer) const = 0;
    virtual void do_bind_input_parameter(statement_handle const & handle, SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, multi_value_buffer & parameter_values) const = 0;
    virtual void do_execute_prepared_statement(statement_handle const & handle) const = 0;
    virtual void do_execute_statement(statement_handle const & handle, std::string const & sql) const = 0;
    virtual bool do_fetch_scroll(statement_handle const & statement_handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const = 0;
    virtual void do_free_statement(statement_handle const & handle, SQLUSMALLINT option) const = 0;
    virtual intptr_t do_get_integer_column_attribute(statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const = 0;
    virtual intptr_t do_get_integer_statement_attribute(statement_handle const & handle, SQLINTEGER attribute) const = 0;
    virtual std::string do_get_string_column_attribute(statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const = 0;
    virtual short int do_number_of_result_columns(statement_handle const & handle) const = 0;
    virtual short int do_number_of_parameters(statement_handle const & handle) const = 0;
    virtual void do_prepare_statement(statement_handle const & handle, std::string const & sql) const = 0;
    virtual void do_prepare_statement(statement_handle const & handle, std::u16string const & sql) const = 0;
    virtual void do_set_statement_attribute(statement_handle const & handle, SQLINTEGER attribute, intptr_t value) const = 0;
    virtual void do_set_statement_attribute(statement_handle const & handle, SQLINTEGER attribute, SQLULEN * pointer) const = 0;
    virtual SQLLEN do_row_count(statement_handle const & handle) const = 0;
    virtual column_description do_describe_column(statement_handle const & handle, SQLUSMALLINT column_id) const = 0;
    virtual column_description do_describe_column_wide(statement_handle const & handle, SQLUSMALLINT column_id) const = 0;
    virtual column_description do_describe_parameter(statement_handle const & handle, SQLUSMALLINT parameter_id) const = 0;
    virtual bool do_more_results(statement_handle const & handle) const = 0;
    virtual bool do_supports_function(connection_handle const & handle, SQLUSMALLINT function_id) const = 0;
};

} }
