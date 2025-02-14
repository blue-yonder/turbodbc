#include "turbodbc/connect.h"
#include <cpp_odbc/make_environment.h>

turbodbc::connection turbodbc::connect(std::string const & connection_string, options options)
{
	auto environment = cpp_odbc::make_environment();
	return {environment->make_connection(connection_string), std::move(options)};
}
