#include <turbodbc/configuration.h>

#include <sqlext.h>

namespace turbodbc {

options::options() :
    read_buffer_size(megabytes(20)),
    parameter_sets_to_buffer(1000),
    use_async_io(false),
    prefer_unicode(false)
{
}

capabilities::capabilities(cpp_odbc::connection const & connection) :
    supports_describe_parameter(connection.supports_function(SQL_API_SQLDESCRIBEPARAM))
{
}

configuration::configuration(turbodbc::options options, turbodbc::capabilities capabilities) :
    options(options),
    capabilities(capabilities)
{
}

}