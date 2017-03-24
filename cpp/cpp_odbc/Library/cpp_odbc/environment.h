#pragma once
/**
 *  @file environment.h
 *  @date 16.05.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate: 2014-11-28 15:54:55 +0100 (Fr, 28 Nov 2014) $
 *  $LastChangedBy: mkoenig $
 *  $LastChangedRevision: 21211 $
 *
 */

#include "cpp_odbc/connection.h"
#include <memory>

namespace cpp_odbc {

/**
 * @brief This interface represents an ODBC environment
 */
class environment : public std::enable_shared_from_this<environment> {
public:
	environment(environment const &) = delete;
	environment & operator=(environment const &) = delete;

	/**
	 * @brief Create a new connection with the given connection string
	 * @param connection_string THis string is used to acquire the ODBC connection
	 * @return A shared pointer to a new connection
	 */
	std::shared_ptr<connection> make_connection(std::string const & connection_string) const;

	/**
	 * @param Set the attribute to the given intptr_t value
	 * @param attribute An ODBC constant which represents the attribute which shall be set
	 * @param value The new value for the attribute
	 */
	void set_attribute(SQLINTEGER attribute, intptr_t value) const;

	virtual ~environment();
protected:
	environment();
private:
	virtual std::shared_ptr<connection> do_make_connection(std::string const & connection_string) const = 0;
	virtual void do_set_attribute(SQLINTEGER attribute, intptr_t value) const = 0;
};

}
