#include <turbodbc/configuration.h>

#include <sqlext.h>

namespace turbodbc {

options::options() :
    read_buffer_size(megabytes(20)),
    parameter_sets_to_buffer(1000),
    varchar_max_character_limit(65535),
    use_async_io(false),
    prefer_unicode(false),
    autocommit(false),
    large_decimals_as_64_bit_types(false),
    limit_varchar_results_to_max(false),
    force_extra_capacity_for_unicode(false),
    fetch_wchar_as_char(false)
{
}

capabilities::capabilities(cpp_odbc::connection const & connection) :
    supports_describe_parameter(connection.supports_function(SQL_API_SQLDESCRIBEPARAM))
{
}

capabilities::capabilities(bool supports_describe_parameter) :
    supports_describe_parameter(supports_describe_parameter)
{
}

configuration::configuration(turbodbc::options options, turbodbc::capabilities capabilities) :
    options(options),
    capabilities(capabilities)
{
}

}
