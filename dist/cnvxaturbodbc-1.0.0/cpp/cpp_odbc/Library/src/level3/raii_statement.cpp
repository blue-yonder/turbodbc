#include "cpp_odbc/level3/raii_statement.h"

#include "cpp_odbc/level3/raii_connection.h"
#include "cpp_odbc/level2/api.h"

#include "sql.h"

#include <iostream>

namespace cpp_odbc { namespace level3 {

raii_statement::raii_statement(std::shared_ptr<raii_connection const> connection) :
connection_(connection),
api_(connection_->get_api()),
handle_(api_->allocate_statement_handle(connection->get_handle()))
{
}

raii_statement::~raii_statement()
{
    try {
        finalize();
    } catch(std::exception const & error) {
        std::cerr << "Error while freeing statement handle: " << error.what() << std::endl;
    }
}

intptr_t raii_statement::do_get_integer_attribute(SQLINTEGER attribute) const
{
    return api_->get_integer_statement_attribute(handle_, attribute);
}

void raii_statement::do_set_attribute(SQLINTEGER attribute, intptr_t value) const
{
    api_->set_statement_attribute(handle_, attribute, value);
}

void raii_statement::do_set_attribute(SQLINTEGER attribute, SQLULEN * pointer) const
{
    api_->set_statement_attribute(handle_, attribute, pointer);
}

void raii_statement::do_execute(std::string const & sql) const
{
    api_->execute_statement(handle_, sql);
}

void raii_statement::do_prepare(std::string const & sql) const
{
    api_->prepare_statement(handle_, sql);
}

void raii_statement::do_prepare(std::u16string const & sql) const
{
    api_->prepare_statement(handle_, sql);
}

void raii_statement::do_bind_input_parameter(SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, cpp_odbc::multi_value_buffer & parameter_values) const
{
    api_->bind_input_parameter(handle_, parameter_id, value_type, parameter_type, digits, parameter_values);
}

void raii_statement::do_unbind_all_parameters() const
{
    api_->free_statement(handle_, SQL_RESET_PARAMS);
}

void raii_statement::do_execute_prepared() const
{
    api_->execute_prepared_statement(handle_);
}

short int raii_statement::do_number_of_columns() const
{
    return api_->number_of_result_columns(handle_);
}

short int raii_statement::do_number_of_parameters() const
{
    return api_->number_of_parameters(handle_);
}

void raii_statement::do_bind_column(SQLUSMALLINT column_id, SQLSMALLINT column_type, cpp_odbc::multi_value_buffer & column_buffer) const
{
    api_->bind_column(handle_, column_id, column_type, column_buffer);
}

void raii_statement::do_unbind_all_columns() const
{
    api_->free_statement(handle_, SQL_UNBIND);
}

bool raii_statement::do_fetch_next() const
{
    SQLSMALLINT const offset = 0;
    return api_->fetch_scroll(handle_, SQL_FETCH_NEXT, offset);
}

void raii_statement::do_close_cursor() const
{
    api_->free_statement(handle_, SQL_CLOSE);
}

intptr_t raii_statement::do_get_integer_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const
{
    return api_->get_integer_column_attribute(handle_, column_id, field_identifier);
}

std::string raii_statement::do_get_string_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const
{
    return api_->get_string_column_attribute(handle_, column_id, field_identifier);
}

SQLLEN raii_statement::do_row_count() const
{
    return api_->row_count(handle_);
}

column_description raii_statement::do_describe_column(SQLUSMALLINT column_id) const
{
    return api_->describe_column(handle_, column_id);
}

column_description raii_statement::do_describe_column_wide(SQLUSMALLINT column_id) const
{
    return api_->describe_column_wide(handle_, column_id);
}

column_description raii_statement::do_describe_parameter(SQLUSMALLINT parameter_id) const
{
    return api_->describe_parameter(handle_, parameter_id);
}

bool raii_statement::do_more_results() const
{
    return api_->more_results(handle_);
}

void raii_statement::do_finalize()
{
    api_->free_handle(handle_);
}

} }
