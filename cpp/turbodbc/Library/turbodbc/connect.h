#pragma once

#include <turbodbc/connection.h>
#include <turbodbc/configuration.h>
#include <string>

namespace turbodbc {

/**
 * @brief Establish a new connection to the database identified by the given
 *        data source name
 * @param connection_string The connection string to connect with the database
 * @param options Options that control how the connection should interact with the database
 */
connection connect(std::string const & connection_string, options options);

}
