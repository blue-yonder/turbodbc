#include "cpp_odbc/level2/level1_connector.h"

#include "cpp_odbc/level1/api.h"
#include "cpp_odbc/level2/string_buffer.h"
#include "cpp_odbc/level2/fixed_length_string_buffer.h"
#include "cpp_odbc/level2/input_string_buffer.h"
#include "cpp_odbc/level2/input_u16string_buffer.h"
#include "cpp_odbc/level2/u16string_buffer.h"

#include "cpp_odbc/error.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include "sqlext.h"

#include <boost/locale.hpp>

#include <sstream>
#include <iostream>
#include <ciso646>

namespace impl {

template <typename Output_Handle, typename Input_Handle>
Output_Handle allocate_handle(
    cpp_odbc::level1::api const & level1,
    cpp_odbc::level2::level1_connector const & level2,
    Input_Handle const & input_handle
)
{
    Output_Handle output_handle = {nullptr};
    auto const return_code = level1.allocate_handle(output_handle.type(), input_handle.handle, &output_handle.handle);

    if (return_code == SQL_SUCCESS) {
        return output_handle;
    } else if (return_code == SQL_SUCCESS_WITH_INFO) {
        auto const record = level2.get_diagnostic_record(input_handle);
        std::cout << "ODBC info: " << record.message
                  << " (status code: " << record.odbc_status_code
                  << ", native error code: " << record.native_error_code << std::endl;
        return output_handle;
    } else {
        throw cpp_odbc::error(level2.get_diagnostic_record(input_handle));
    }
}

template <typename Handle>
void free_handle(
    cpp_odbc::level1::api const & level1,
    cpp_odbc::level2::level1_connector const & level2,
    Handle & handle
)
{
    auto const return_code = level1.free_handle(handle.type(), handle.handle);

    if (return_code == SQL_ERROR) {
        throw cpp_odbc::error(level2.get_diagnostic_record(handle));
    }
}

template <typename Handle>
void throw_on_error(SQLRETURN return_code, cpp_odbc::level2::level1_connector const & level2, Handle const & handle)
{
    if (return_code == SQL_ERROR) {
        throw cpp_odbc::error(level2.get_diagnostic_record(handle));
    }
}


cpp_odbc::level2::diagnostic_record get_diagnostic_record(
    cpp_odbc::level1::api const & api,
    signed short int type,
    void * handle
)
{
    cpp_odbc::level2::fixed_length_string_buffer<5> status_code;
    SQLINTEGER native_error = 0;
    cpp_odbc::level2::string_buffer message(SQL_MAX_MESSAGE_LENGTH);
    SQLSMALLINT const record_id = 1;

    auto const return_code = api.get_diagnostic_record(type, handle, record_id, status_code.data_pointer(), &native_error, message.data_pointer(), message.capacity(), message.size_pointer());

    if (return_code == SQL_SUCCESS) {
        return {status_code, native_error, message};
    } else {
        throw cpp_odbc::error("Obtaining diagnostic record from unixODBC handle failed");
    }
}

}

namespace cpp_odbc { namespace level2 {

level1_connector::level1_connector(std::shared_ptr<level1::api const> level1_api) :
level1_api_(std::move(level1_api))
    {
    }

statement_handle level1_connector::do_allocate_statement_handle(connection_handle const & input_handle) const
{
    return impl::allocate_handle<statement_handle>(*level1_api_, *this, input_handle);
}

connection_handle level1_connector::do_allocate_connection_handle(environment_handle const & input_handle) const
{
    return impl::allocate_handle<connection_handle>(*level1_api_, *this, input_handle);
}

environment_handle level1_connector::do_allocate_environment_handle() const
{
    // separate treatment here because SQL_NULL_HANDLE does not hold diagnostic_record
    environment_handle output_handle = {nullptr};
    auto const return_code = level1_api_->allocate_handle(output_handle.type(), SQL_NULL_HANDLE, &output_handle.handle);

    if ((return_code == SQL_SUCCESS) or (return_code == SQL_SUCCESS_WITH_INFO)){
        return output_handle;
    } else {
        throw cpp_odbc::error("Could not allocate environment handle");
    }
}

void level1_connector::do_free_handle(statement_handle & handle) const
{
    impl::free_handle(*level1_api_, *this, handle);
}

void level1_connector::do_free_handle(connection_handle & handle) const
{
    impl::free_handle(*level1_api_, *this, handle);
}

void level1_connector::do_free_handle(environment_handle & handle) const
{
    impl::free_handle(*level1_api_, *this, handle);
}

diagnostic_record level1_connector::do_get_diagnostic_record(statement_handle const & handle) const
{
    return impl::get_diagnostic_record(*level1_api_, handle.type(), handle.handle);
}

diagnostic_record level1_connector::do_get_diagnostic_record(connection_handle const & handle) const
{
    return impl::get_diagnostic_record(*level1_api_, handle.type(), handle.handle);
}

diagnostic_record level1_connector::do_get_diagnostic_record(environment_handle const & handle) const
{
    return impl::get_diagnostic_record(*level1_api_, handle.type(), handle.handle);
}

void level1_connector::do_set_environment_attribute(environment_handle const & handle, SQLINTEGER attribute, intptr_t value) const
{
    // ODBC's interface transfers integer values disguised as a pointer.
    auto const value_ptr = reinterpret_cast<SQLPOINTER>(value);
    auto const return_code = level1_api_->set_environment_attribute(handle.handle, attribute, value_ptr, 0);

    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_set_connection_attribute(connection_handle const & handle, SQLINTEGER attribute, intptr_t value) const
{
    // ODBC's interface transfers integer values disguised as a pointer.
    auto const value_ptr = reinterpret_cast<SQLPOINTER>(value);
    auto const return_code = level1_api_->set_connection_attribute(handle.handle, attribute, value_ptr, 0);

    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_establish_connection(connection_handle & handle, std::string const & connection_string) const
{
    cpp_odbc::level2::input_string_buffer input_connection_string(connection_string);
    cpp_odbc::level2::string_buffer output_connection_string(1024);

    auto const return_code = level1_api_->establish_connection(
        handle.handle,
        nullptr,
        input_connection_string.data_pointer(),
        input_connection_string.size(),
        output_connection_string.data_pointer(),
        output_connection_string.capacity(),
        output_connection_string.size_pointer(),
        SQL_DRIVER_NOPROMPT);

    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_disconnect(connection_handle & handle) const
{
    auto const return_code = level1_api_->disconnect(handle.handle);
    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_end_transaction(connection_handle const & handle, SQLSMALLINT completion_type) const
{
    auto const return_code = level1_api_->end_transaction(handle.type(), handle.handle, completion_type);
    impl::throw_on_error(return_code, *this, handle);
}

std::string level1_connector::do_get_string_connection_info(connection_handle const & handle, SQLUSMALLINT info_type) const
{
    cpp_odbc::level2::string_buffer output_connection_string(1024);
    auto const return_code = level1_api_->get_connection_info(
        handle.handle,
        info_type,
        output_connection_string.data_pointer(),
        output_connection_string.capacity(),
        output_connection_string.size_pointer()
    );

    impl::throw_on_error(return_code, *this, handle);
    return output_connection_string;
}

SQLUINTEGER level1_connector::do_get_integer_connection_info(connection_handle const & handle, SQLUSMALLINT info_type) const
{
    SQLUINTEGER destination = 0;
    auto const return_code = level1_api_->get_connection_info(
        handle.handle,
        info_type,
        &destination,
        0,
        nullptr
    );

    impl::throw_on_error(return_code, *this, handle);
    return destination;
}

void level1_connector::do_bind_column(statement_handle const & handle, SQLUSMALLINT column_id, SQLSMALLINT column_type, multi_value_buffer & column_buffer) const
{
    auto const return_code = level1_api_->bind_column(
        handle.handle,
        column_id,
        column_type,
        column_buffer.data_pointer(),
        column_buffer.capacity_per_element(),
        column_buffer.indicator_pointer()
    );

    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_bind_input_parameter(statement_handle const & handle, SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, multi_value_buffer & parameter_values) const
{
    auto const return_code = level1_api_->bind_parameter(
        handle.handle,
        parameter_id,
        SQL_PARAM_INPUT,
        value_type,
        parameter_type,
        parameter_values.capacity_per_element(),
        digits,
        parameter_values.data_pointer(),
        parameter_values.capacity_per_element(),
        parameter_values.indicator_pointer()
    );

    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_execute_prepared_statement(statement_handle const & handle) const
{
    auto const return_code = level1_api_->execute_prepared_statement(handle.handle);
    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_execute_statement(statement_handle const & handle, std::string const & sql) const
{
    cpp_odbc::level2::input_string_buffer buffered(sql);
    auto const return_code = level1_api_->execute_statement(handle.handle, buffered.data_pointer(), buffered.size());
    impl::throw_on_error(return_code, *this, handle);
}

bool level1_connector::do_fetch_scroll(statement_handle const & handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const
{
    auto const return_code = level1_api_->fetch_scroll(handle.handle, fetch_orientation, fetch_offset);
    if (return_code == SQL_NO_DATA) {
        return false;
    } else {
        impl::throw_on_error(return_code, *this, handle);
        return true;
    }
}

void level1_connector::do_free_statement(statement_handle const & handle, SQLUSMALLINT option) const
{
    auto const return_code = level1_api_->free_statement(handle.handle, option);
    impl::throw_on_error(return_code, *this, handle);
}

intptr_t level1_connector::do_get_integer_column_attribute(statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const
{
    SQLLEN attribute_value = 0;
    auto const return_code = level1_api_->column_attribute(
        handle.handle,
        column_id,
        field_identifier,
        nullptr,
        0,
        nullptr,
        &attribute_value
    );

    impl::throw_on_error(return_code, *this, handle);

    return attribute_value;
}

intptr_t level1_connector::do_get_integer_statement_attribute(statement_handle const & handle, SQLINTEGER attribute) const
{
    SQLLEN attribute_value = 0;
    auto const return_code = level1_api_->get_statement_attribute(
        handle.handle,
        attribute,
        &attribute_value,
        0,
        nullptr
    );

    impl::throw_on_error(return_code, *this, handle);

    return attribute_value;
}

std::string level1_connector::do_get_string_column_attribute(statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const
{
    cpp_odbc::level2::string_buffer attribute_value(1024);
    auto const return_code = level1_api_->column_attribute(
        handle.handle,
        column_id,
        field_identifier,
        attribute_value.data_pointer(),
        attribute_value.capacity(),
        attribute_value.size_pointer(),
        nullptr
    );

    impl::throw_on_error(return_code, *this, handle);

    return attribute_value;
}

short int level1_connector::do_number_of_result_columns(statement_handle const & handle) const
{
    SQLSMALLINT columns = 0;
    auto const return_code = level1_api_->number_of_result_columns(handle.handle, &columns);

    impl::throw_on_error(return_code, *this, handle);

    return columns;
}

short int level1_connector::do_number_of_parameters(statement_handle const & handle) const
{
    SQLSMALLINT columns = 0;
    auto const return_code = level1_api_->number_of_parameters(handle.handle, &columns);

    impl::throw_on_error(return_code, *this, handle);

    return columns;
}

void level1_connector::do_prepare_statement(statement_handle const & handle, std::string const & sql) const
{
    cpp_odbc::level2::input_string_buffer buffered(sql);
    auto const return_code = level1_api_->prepare_statement(handle.handle, buffered.data_pointer(), buffered.size());
    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_prepare_statement(statement_handle const & handle, std::u16string const & sql) const
{
    cpp_odbc::level2::input_u16string_buffer buffered(sql);
    auto const return_code = level1_api_->prepare_statement(handle.handle, reinterpret_cast<SQLWCHAR *>(buffered.data_pointer()), buffered.size());
    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_set_statement_attribute(statement_handle const & handle, SQLINTEGER attribute, intptr_t value) const
{
    auto value_as_pointer = reinterpret_cast<void*>(value); // damn C API
    auto const return_code = level1_api_->set_statement_attribute(handle.handle, attribute, value_as_pointer, SQL_IS_INTEGER);
    impl::throw_on_error(return_code, *this, handle);
}

void level1_connector::do_set_statement_attribute(statement_handle const & handle, SQLINTEGER attribute, SQLULEN * pointer) const
{
    auto const return_code = level1_api_->set_statement_attribute(handle.handle, attribute, pointer, SQL_IS_POINTER);
    impl::throw_on_error(return_code, *this, handle);
}

SQLLEN level1_connector::do_row_count(statement_handle const & handle) const
{
    SQLLEN count = 0;
    auto const return_code = level1_api_->row_count(handle.handle, &count);

    impl::throw_on_error(return_code, *this, handle);

    return count;
}

column_description level1_connector::do_describe_column(statement_handle const & handle, SQLUSMALLINT column_id) const
{
    cpp_odbc::level2::string_buffer name(256);
    SQLSMALLINT data_type = 0;
    SQLULEN size = 0;
    SQLSMALLINT decimal_digits = 0;
    SQLSMALLINT nullable = 0;

    auto const return_code = level1_api_->describe_column(handle.handle, column_id, name.data_pointer(), name.capacity(), name.size_pointer(), &data_type, &size, &decimal_digits, &nullable);

    bool const allows_nullable = (nullable == SQL_NO_NULLS) ? false : true;

    impl::throw_on_error(return_code, *this, handle);

    return {static_cast<std::string>(name), data_type, size, decimal_digits, allows_nullable};
}

column_description level1_connector::do_describe_column_wide(statement_handle const & handle, SQLUSMALLINT column_id) const
{
    cpp_odbc::level2::u16string_buffer name(256);
    SQLSMALLINT data_type = 0;
    SQLULEN size = 0;
    SQLSMALLINT decimal_digits = 0;
    SQLSMALLINT nullable = 0;

    auto const return_code = level1_api_->describe_column(handle.handle, column_id, name.data_pointer(), name.capacity(), name.size_pointer(), &data_type, &size, &decimal_digits, &nullable);

    bool const allows_nullable = (nullable == SQL_NO_NULLS) ? false : true;

    impl::throw_on_error(return_code, *this, handle);

    auto const utf8_name = boost::locale::conv::utf_to_utf<char>(static_cast<std::u16string>(name));
    return {utf8_name, data_type, size, decimal_digits, allows_nullable};
}

column_description level1_connector::do_describe_parameter(statement_handle const & handle, SQLUSMALLINT parameter_id) const
{
    SQLSMALLINT data_type = 0;
    SQLULEN size = 0;
    SQLSMALLINT decimal_digits = 0;
    SQLSMALLINT nullable = 0;

    auto const return_code = level1_api_->describe_parameter(handle.handle, parameter_id, &data_type, &size, &decimal_digits, &nullable);

    bool const allows_nullable = (nullable == SQL_NO_NULLS) ? false : true;

    impl::throw_on_error(return_code, *this, handle);

    std::ostringstream name;
    name << "parameter_" << parameter_id;
    return {name.str(), data_type, size, decimal_digits, allows_nullable};
}

bool level1_connector::do_more_results(statement_handle const & handle) const
{
    auto const return_code = level1_api_->more_results(handle.handle);
    impl::throw_on_error(return_code, *this, handle);
    return return_code != SQL_NO_DATA;
}

bool level1_connector::do_supports_function(connection_handle const & handle, SQLUSMALLINT function_id) const
{
    SQLUSMALLINT is_supported = 0;
    auto const return_code = level1_api_->get_functions(handle.handle, function_id, &is_supported);
    impl::throw_on_error(return_code, *this, handle);
    return (is_supported == SQL_TRUE);
}

} }
