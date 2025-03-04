#pragma once

#include <turbodbc/result_sets/result_set.h>

namespace turbodbc { namespace result_sets {

/**
 * @brief This class lets you use a base result_set in a row-based fashion
 *        without worrying about batches
 */
class row_based_result_set {
public:
	/**
	 * @brief Create a new row_based_result_set which presents data contained
	 *        in the base result set in a row-based fashion
	 */
	row_based_result_set(result_set & base);

	/**
	 * @brief Retrieve information about each column in the result set
	 */
	std::vector<column_info> get_column_info() const;

	/**
	 * @brief Retrieve buffer elements which belong to the next row.
	 * @return Returned vector is empty in case there is no additional row
	 */
	std::vector<cpp_odbc::buffer_element> fetch_row();

private:
	std::vector<cpp_odbc::buffer_element> get_current_row();

	result_set & base_;
	std::size_t rows_in_batch_;
	std::size_t current_row_in_batch_;
	std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> buffers_;
};

} }
