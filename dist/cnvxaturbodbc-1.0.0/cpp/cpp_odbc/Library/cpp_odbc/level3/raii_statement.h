#pragma once

#include "cpp_odbc/statement.h"

#include "cpp_odbc/level2/handles.h"

#include <memory>

namespace cpp_odbc { namespace level2 {
	class api;
} }

namespace cpp_odbc { namespace level3 {

class raii_connection;

/**
 * A concrete implementation of the statement interface. It manages the lifetime of
 * the underlying level2::statement_handle and forwards all calls to the level2 API
 * associated with the given connection.
 */
class raii_statement : public statement {
public:
	/**
	 * @brief Create a new raii_statement
	 * @param connection The statement lives in this connection. The connection is kept alive for
	 *                   the life time of this object. The connection also contains the level2 API
	 *                   to which all calls are forwarded.
	 */
	raii_statement(std::shared_ptr<raii_connection const> connection);

	virtual ~raii_statement();
private:
	intptr_t do_get_integer_attribute(SQLINTEGER attribute) const final;
	void do_set_attribute(SQLINTEGER attribute, intptr_t value) const final;
	void do_set_attribute(SQLINTEGER attribute, SQLULEN * pointer) const final;
	void do_execute(std::string const & sql) const final;
	void do_prepare(std::string const & sql) const final;
	void do_prepare(std::u16string const & sql) const final;
	void do_bind_input_parameter(SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, cpp_odbc::multi_value_buffer & parameter_values) const final;
	void do_unbind_all_parameters() const final;
	void do_execute_prepared() const final;

	short int do_number_of_columns() const final;
	short int do_number_of_parameters() const final;
	void do_bind_column(SQLUSMALLINT column_id, SQLSMALLINT column_type, cpp_odbc::multi_value_buffer & column_buffer) const final;
	void do_unbind_all_columns() const final;
	bool do_fetch_next() const final;
	void do_close_cursor() const final;

	intptr_t do_get_integer_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const final;
	std::string do_get_string_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const final;
	SQLLEN do_row_count() const final;
	column_description do_describe_column(SQLUSMALLINT column_id) const final;
	column_description do_describe_column_wide(SQLUSMALLINT column_id) const final;
	column_description do_describe_parameter(SQLUSMALLINT parameter_id) const final;
	bool do_more_results() const final;
	void do_finalize() final;

	std::shared_ptr<raii_connection const> connection_;
	std::shared_ptr<level2::api const> api_;
	level2::statement_handle handle_;
};


} }
