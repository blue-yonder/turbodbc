#pragma once

#include <turbodbc/description.h>

namespace turbodbc {

/**
 * @brief Represents a description to bind a buffer holding integer values
 */
class integer_description : public description {
public:
	integer_description();
	integer_description(std::string name, bool supports_null);
	~integer_description();
private:
	std::size_t do_element_size() const final;
	SQLSMALLINT do_column_c_type() const final;
	SQLSMALLINT do_column_sql_type() const final;
	SQLSMALLINT do_digits() const final;
	type_code do_get_type_code() const final;
};

}
