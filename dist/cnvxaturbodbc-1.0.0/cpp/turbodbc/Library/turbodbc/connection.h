#pragma once

#include <turbodbc/cursor.h>
#include <cpp_odbc/connection.h>
#include <memory>

#include <turbodbc/configuration.h>

namespace turbodbc {

/**
 * @brief This class is provides basic functionality required by python's
 *        connection class as specified by the database API version 2.
 *        Additional wrapping may be required.
 */
class connection {
public:
    /**
     * @brief Construct a new connection based on the given low-level connection
     */
    connection(std::shared_ptr<cpp_odbc::connection const> low_level_connection,
               options options);

    /**
     * @brief Commit all operations which have been performed since the last commit
     *        or rollback
     */
    void commit() const;

    /**
     * @brief Roll back all operations which have been performed since the last commit
     *        or rollback
     */
    void rollback() const;

    /**
     * @brief Enable or disable autocommit mode
     * @param enabled Set to true to enable autocommit
     */
    void set_autocommit(bool enabled);

    /**
     * @brief Returns whether autocommit is currently enabled
     */
    bool autocommit_enabled() const;

    /**
     * @brief Create a new cursor object associated with this connection
     */
    turbodbc::cursor make_cursor() const;
private:
    std::shared_ptr<cpp_odbc::connection const> connection_;
    turbodbc::configuration configuration;
};

}
