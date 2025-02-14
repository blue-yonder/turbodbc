#pragma once

#include "cpp_odbc/multi_value_buffer.h"
#include "cpp_odbc/column_description.h"

#include "sql.h"

#include <string>

namespace cpp_odbc {

/**
 * @brief An interface representing a statement on an ODBC connection
 */
class statement {
public:
    statement(statement const &) = delete;
    statement & operator=(statement const &) = delete;

    /**
     * @brief Retrieve the value of statement attribute as an integer
     * @param attribute A constant representing the requested attribute
     */
    intptr_t get_integer_attribute(SQLINTEGER attribute) const;

    /**
     * @brief Set the value of statement attribute
     * @param attribute A constant representing the requested attribute
     * @param value The new value for this attribute
     */
    void set_attribute(SQLINTEGER attribute, intptr_t value) const;

    /**
     * @brief Set the pointer of the given statement attribute
     * @param attribute A constant representing the requested attribute
     * @param pointer The new pointer for this attribute
     */
    void set_attribute(SQLINTEGER attribute, SQLULEN * pointer) const;

    /**
     * @brief Execute the given SQL string directly without preparing it on the client side.
     * @param sql The SQL query which shall be executed
     */
    void execute(std::string const & sql) const;

    /**
     * @brief Prepare the given SQL string. Call execute_prepared() to actually execute the query.
     * @param sql The SQL query which shall be executed
     */
    void prepare(std::string const & sql) const;
    void prepare(std::u16string const & sql) const;

    /**
     * @brief Assign a buffer to contain values for a given input parameter.
     * @param parameter_id The ID of the parameter
     * @param value_type The C data type identifier of the buffer. See unixODBC's SQLBindParameter() documentation
     * @param parameter_type The SQL data type identifier of the buffer. See unixODBC's SQLBindParameter() documentation
     * @param parameter_values The buffer which shall be bound as a parameter
     */
    void bind_input_parameter(SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, cpp_odbc::multi_value_buffer & parameter_values) const;

    /**
     * @brief Unbind all parameters currently bound to the statement
     */
    void unbind_all_parameters() const;

    /**
     * @brief Executes an SQL query which has previously been prepared. Call after prepare()
     */
    void execute_prepared() const;

    /**
     * @brief Retrieve the number of columns in the result set. Call after execute() or execute_prepared()
     * @return The number of columns in the result set
     */
    short int number_of_columns() const;

    /**
     * @brief Retrieve the number of parameters in the prepared statement. Call after prepare(), but before execute_prepared()
     * @return The number of parameters in the prepared statement
     */
    short int number_of_parameters() const;

    /**
     * @brief Bind a buffer to a column of the statement
     * @param column_id The column identifier with which we want to associate the buffer
     * @param column_type The identifier of the C data type of the column. See unixODBC's SQLBindCol() documentation.
     * @param column_buffer A buffer which will be filled with data whenever fetch_scroll() is called.
     */
    void bind_column(SQLUSMALLINT column_id, SQLSMALLINT column_type, cpp_odbc::multi_value_buffer & column_buffer) const;

    /**
     * @brief Unbind all columns currently bound to the statement
     */
    void unbind_all_columns() const;

    /**
     * @brief Fetch the next elements of the result of the previously executed statement and
     *        store it in previously bound buffers
     * @return true if fetch retrieved data, else false
     */
    bool fetch_next() const;

    /**
     * @brief Close the currently open cursor
     */
    void close_cursor() const;

    /**
     * @brief Retrieve the value of a column attribute as an integer
     * @param column_id The column identifier
     * @param field_identifier A constant representing the requested attribute
     */
    intptr_t get_integer_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const;

    /**
     * @brief Retrieve the value of a column attribute as a string
     * @param column_id The column identifier
     * @param field_identifier A constant representing the requested attribute
     * @return The attribute's value
     */
    std::string get_string_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const;

    /**
     * @brief Return number of rows in result set
     */
    SQLLEN row_count() const;

    /**
     * @brief Retrieve the description of a column
     * @param column_id The column identifier
     * @return A description of the column
     */
    column_description describe_column(SQLUSMALLINT column_id) const;

    /**
     * @brief Retrieve the description of a column with explicit support for wide column names
     * @param column_id The column identifier
     * @return A description of the column
     */
    column_description describe_column_wide(SQLUSMALLINT column_id) const;

    /**
     * @brief Retrieve the description of a parameter
     * @param parameter_id The parameter identifier
     * @return A description of the parameter
     */
    column_description describe_parameter(SQLUSMALLINT parameter_id) const;

    /**
     * @brief Make more results (created by multiple queries or parameter sets)
     *        available
     * @brief Return true if another result is found
     */
    bool more_results() const;

    virtual ~statement();

    /**
     * @brief Close all odbc ressources associated with this instance
     * 
     * This should only be called directly prior to the destructor; it is not
     * valid to call any other method (except the destructor) after calling finalize.
     */
    void finalize();
protected:
    statement();
private:
    virtual intptr_t do_get_integer_attribute(SQLINTEGER attribute) const = 0;
    virtual void do_set_attribute(SQLINTEGER attribute, intptr_t value) const = 0;
    virtual void do_set_attribute(SQLINTEGER attribute, SQLULEN * pointer) const = 0;
    virtual void do_execute(std::string const & sql) const = 0;
    virtual void do_prepare(std::string const & sql) const = 0;
    virtual void do_prepare(std::u16string const & sql) const = 0;
    virtual void do_bind_input_parameter(SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, cpp_odbc::multi_value_buffer & parameter_values) const = 0;
    virtual void do_unbind_all_parameters() const = 0;
    virtual void do_execute_prepared() const = 0;

    virtual short int do_number_of_columns() const = 0;
    virtual short int do_number_of_parameters() const = 0;
    virtual void do_bind_column(SQLUSMALLINT column_id, SQLSMALLINT column_type, cpp_odbc::multi_value_buffer & column_buffer) const = 0;
    virtual void do_unbind_all_columns() const = 0;
    virtual bool do_fetch_next() const = 0;
    virtual void do_close_cursor() const = 0;

    virtual intptr_t do_get_integer_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const = 0;
    virtual std::string do_get_string_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const = 0;
    virtual SQLLEN do_row_count() const = 0;
    virtual column_description do_describe_column(SQLUSMALLINT column_id) const = 0;
    virtual column_description do_describe_column_wide(SQLUSMALLINT column_id) const = 0;
    virtual column_description do_describe_parameter(SQLUSMALLINT parameter_id) const = 0;
    virtual bool do_more_results() const = 0;

    virtual void do_finalize() = 0;

    bool finalized = false;
};

}
