#pragma once

#include <turbodbc/column_info.h>

#include <cpp_odbc/multi_value_buffer.h>

#include <vector>
#include <functional>

namespace turbodbc { namespace result_sets {

/**
 * @brief A base class for result sets
 */
class result_set {
public:
	virtual ~result_set();

	/**
	 * @brief Tell the result set to fetch the next batch of rows
	 *
	 *        Invalidates buffers previously retrieved with get_buffers()
	 *
	 * @return The number of rows which came with this batch
	 */
	std::size_t fetch_next_batch();

	/**
	 * @brief Retrieve information about each column in the result set
	 */
	std::vector<column_info> get_column_info() const;

	/**
	 * @brief Get references to buffers representing chunked columns.
	 *        A call to fetch_next_batch may invalidate the references!
	 */
	std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> get_buffers() const;

protected:
	result_set();

private:
	result_set(result_set const &) = delete;
	result_set & operator=(result_set const &) = delete;

	virtual std::size_t do_fetch_next_batch() = 0;
	virtual std::vector<column_info> do_get_column_info() const = 0;
	virtual std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> do_get_buffers() const = 0;
};

} }
