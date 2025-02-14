#include <turbodbc/result_sets/field_result_set.h>

#include <turbodbc/make_field_translator.h>

#include <ciso646>

namespace turbodbc { namespace result_sets {

field_result_set::field_result_set(result_set & base) :
	row_based_(base)
{
	for (auto const & info : row_based_.get_column_info()) {
		translators_.emplace_back(make_field_translator(info));
	}
}

std::vector<column_info> field_result_set::get_column_info() const
{
	return row_based_.get_column_info();
}


std::vector<nullable_field> field_result_set::fetch_row()
{
	auto const row = row_based_.fetch_row();
	if (not row.empty()) {
		std::vector<nullable_field> results;
		results.reserve(translators_.size());

		for (std::size_t i = 0; i != translators_.size(); ++i) {
			results.emplace_back(translators_[i]->make_field(row[i]));
		}
		return results;
	} else {
		return {};
	}
}



} }
