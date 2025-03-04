#pragma once

#ifdef _WIN32
#include <windows.h>
#endif
#include "sqltypes.h"

#include <string>
#include <iosfwd>

namespace cpp_odbc {


/**
 * @brief Information which fully describes a column of a result set
 */
struct column_description {
	std::string name;           ///< Column name
	SQLSMALLINT data_type;      ///< SQL data type constant
	SQLULEN size;               ///< Size of column. Corresponds to size of strings or precision of numeric types
	SQLSMALLINT decimal_digits; ///< Decimal digits of column. Corresponds to scale of numeric types and precision of timestamps
	bool allows_null_values;	///< True if NULL values are allowed in the column
};

/**
 * @brief Equality operator for column_description
 */
bool operator==(column_description const & lhs, column_description const & rhs);

/**
 * @brief Output operator for column_description
 */
std::ostream & operator<<(std::ostream & destination, column_description const & source);


}
