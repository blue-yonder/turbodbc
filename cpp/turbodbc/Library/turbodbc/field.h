#pragma once

#include <boost/variant/variant.hpp>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>

namespace turbodbc {

/**
 * @brief This type represents a single field in a table, i.e., the data associated
 *        with a given row and column
 */
using field = boost::variant<	long,
								std::string,
								bool,
								double,
								std::tm,
								boost::posix_time::ptime>;

/**
 * @brief This type represents a single nullable field in a table, i.e., the data associated
 *        with a given row and column.
 */
using nullable_field = boost::optional<field>;

}
