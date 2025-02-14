#pragma once

#include <turbodbc/result_sets/row_based_result_set.h>
#include <turbodbc/field.h>
#include <turbodbc/field_translator.h>
#include <memory>

namespace turbodbc { namespace result_sets {

/**
 * @brief This class adapts a result_set to provide row-based access via
 *        the nullable_field data type
 */
class field_result_set {
public:
	/**
	 * @brief Create a new field_result_set which presents data contained
	 *        in the base result set in a row-based fashion
	 */
	field_result_set(result_set & base);

	/**
	 * @brief Retrieve information about each column in the result set
	 */
	std::vector<column_info> get_column_info() const;

	/**
	 * @brief Retrieve nullable fields which belong to the next row.
	 * @return Returned vector is empty in case there is no additional row
	 */
	std::vector<nullable_field> fetch_row();

private:
	row_based_result_set row_based_;
	std::vector<std::unique_ptr<field_translator const>> translators_;
};

} }
