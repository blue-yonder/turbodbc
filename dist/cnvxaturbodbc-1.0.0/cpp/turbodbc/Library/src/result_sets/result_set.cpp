#include <turbodbc/result_sets/result_set.h>

namespace turbodbc { namespace result_sets {

result_set::result_set() = default;
result_set::~result_set() = default;

std::size_t result_set::fetch_next_batch()
{
	return do_fetch_next_batch();
}

std::vector<column_info> result_set::get_column_info() const
{
	return do_get_column_info();
}

std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> result_set::get_buffers() const
{
	return do_get_buffers();
}


} }
