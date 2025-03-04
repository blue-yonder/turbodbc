#include "turbodbc/connection.h"
#include <sqlext.h>

namespace turbodbc {

connection::connection(std::shared_ptr<cpp_odbc::connection const> low_level_connection,
                       options options) :
    connection_(low_level_connection),
    configuration(std::move(options), capabilities(*connection_))
{
    if (configuration.options.autocommit) {
        connection_->set_attribute(SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_ON);
    } else {
        connection_->set_attribute(SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
    }
}

void connection::commit() const
{
    connection_->commit();
}

void connection::rollback() const
{
    connection_->rollback();
}

void connection::set_autocommit(bool enabled)
{
    if (enabled) {
        connection_->set_attribute(SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_ON);
    } else {
        connection_->set_attribute(SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
    }
    configuration.options.autocommit = enabled;
}

bool connection::autocommit_enabled() const
{
    return configuration.options.autocommit;
}

cursor connection::make_cursor() const
{
    return {connection_, configuration};
}

}
