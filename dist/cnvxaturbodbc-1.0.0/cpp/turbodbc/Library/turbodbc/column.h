#pragma once

#include <cpp_odbc/statement.h>
#include <turbodbc/field.h>
#include <turbodbc/description.h>
#include <turbodbc/column_info.h>

namespace turbodbc {

/**
 * @brief This interface represents a single column of a result set.
 */
class column {
public:
	/**
	 * @brief Create a new column
	 * @param statement The statement a subsequent call to bind() to column would attach to
	 * @param one_based_index One-based column index for bind() command
	 * @param buffered_rows Number of rows for which the buffer should be allocated
	 * @param desription Description concerning data type of column
	 */
	column(cpp_odbc::statement const & statement, std::size_t one_based_index, std::size_t buffered_rows, std::unique_ptr<description const> description);

	/**
	 * @brief Move constructor
	 */
	column(column && other);

	/**
	 * (Re-) Bind the column to the statement
	 */
	void bind();

	column_info get_info() const;

	/**
	 * @brief Retrieve a reference to the internal buffer
	 */
	cpp_odbc::multi_value_buffer const & get_buffer() const;

	~column();
private:
	cpp_odbc::statement const & statement_;
	std::size_t one_based_index_;
	std::unique_ptr<description const> description_;
	cpp_odbc::multi_value_buffer buffer_;
};


}
