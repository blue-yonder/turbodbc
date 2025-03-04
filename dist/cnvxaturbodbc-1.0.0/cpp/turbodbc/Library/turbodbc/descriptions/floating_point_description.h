#pragma once

#include <turbodbc/description.h>

namespace turbodbc {

/**
 * @brief Represents a description to bind a buffer holding double precision
 *        floating point values
 */
class floating_point_description : public description {
public:
	floating_point_description();
	floating_point_description(std::string name, bool supports_null);
	~floating_point_description();
private:
	std::size_t do_element_size() const final;
	SQLSMALLINT do_column_c_type() const final;
	SQLSMALLINT do_column_sql_type() const final;
	SQLSMALLINT do_digits() const final;
	type_code do_get_type_code() const final;
};


}
