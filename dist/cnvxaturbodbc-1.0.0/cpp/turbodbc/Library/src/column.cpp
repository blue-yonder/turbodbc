#include <turbodbc/column.h>

namespace turbodbc {

column::column(cpp_odbc::statement const & statement, std::size_t one_based_index, std::size_t buffered_rows, std::unique_ptr<description const> description) :
	statement_(statement),
	one_based_index_(one_based_index),
	description_(std::move(description)),
	buffer_(description_->element_size(), buffered_rows)
{
}

void column::bind()
{
	statement_.bind_column(one_based_index_, description_->column_c_type(), buffer_);
}

column::column(column && other) :
	statement_(other.statement_),
	one_based_index_(other.one_based_index_),
	description_(std::move(other.description_)),
	buffer_(std::move(other.buffer_))
{
}

column::~column() = default;

column_info column::get_info() const
{
	return {description_->name(), description_->get_type_code(), description_->element_size(), description_->supports_null_values()};
}

cpp_odbc::multi_value_buffer const & column::get_buffer() const
{
	return buffer_;
}

}
