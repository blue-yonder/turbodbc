#pragma once

#include <turbodbc/field.h>
#include <turbodbc/type_code.h>
#include <cpp_odbc/multi_value_buffer.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <sqltypes.h>

namespace turbodbc {

/**
 * @brief Represents all information to bind a buffer to a column or parameter and
 *        how to store/load values
 */
class description {
public:
	/**
	 * @brief Returns the size of an element in a buffer
	 */
	std::size_t element_size() const;

	/**
	 * @brief Returns the type code for the associated C data type
	 */
	SQLSMALLINT column_c_type() const;

	/**
	 * @brief Returns the type code for the associated SQL column data type
	 */
	SQLSMALLINT column_sql_type() const;

	/**
	 * @brief Returns the number of digits a parameter supports
	 */
	SQLSMALLINT digits() const;

	/**
	 * @brief Retrieve a code which indicates this field's type
	 */
	type_code get_type_code() const;

	/**
	 * @brief Retrieve the name associated with the described column or parameter
	 */
	std::string const & name() const;

	/**
	 * @brief Retrieve whether null values are supported or not
	 */
	bool supports_null_values() const;

	description (description const &) = delete;
	description & operator=(description const &) = delete;

	virtual ~description();
protected:
	description();
	description(std::string name, bool supports_null);
private:
	virtual std::size_t do_element_size() const = 0;
	virtual SQLSMALLINT do_column_c_type() const = 0;
	virtual SQLSMALLINT do_column_sql_type() const = 0;
	virtual SQLSMALLINT do_digits() const = 0;
	virtual type_code do_get_type_code() const = 0;

	std::string name_;
	bool supports_null_;
};

}
