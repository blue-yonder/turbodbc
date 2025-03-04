#include <turbodbc/result_sets/row_based_result_set.h>

namespace turbodbc { namespace result_sets {

row_based_result_set::row_based_result_set(result_set & base) :
	base_(base),
	rows_in_batch_(0),
	current_row_in_batch_(0)
{
}

std::vector<column_info> row_based_result_set::get_column_info() const
{
	return base_.get_column_info();
}


std::vector<cpp_odbc::buffer_element> row_based_result_set::fetch_row()
{
	if (current_row_in_batch_ == rows_in_batch_) {
		rows_in_batch_ = base_.fetch_next_batch();
		buffers_ = base_.get_buffers(); // fetch_next_batch invalidates buffers
		current_row_in_batch_ = 0;
	}

	if (rows_in_batch_ != 0) {
		return get_current_row();
	} else {
		return {};
	}
}


std::vector<cpp_odbc::buffer_element> row_based_result_set::get_current_row()
{
	std::vector<cpp_odbc::buffer_element> elements;
	for (auto const & buffer : buffers_) {
		elements.push_back(buffer.get()[current_row_in_batch_]);
	}
	current_row_in_batch_++;

	return elements;
}


} }
