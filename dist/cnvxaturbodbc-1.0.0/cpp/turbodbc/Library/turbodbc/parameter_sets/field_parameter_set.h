#pragma once

#include <turbodbc/parameter_sets/bound_parameter_set.h>
#include <vector>

namespace turbodbc {

class field_parameter_set {
public:
	field_parameter_set(bound_parameter_set & parameters);

	void add_parameter_set(std::vector<nullable_field> const & parameter_set);

	void flush();

	~field_parameter_set();

private:
	void check_parameter_set(std::vector<nullable_field> const & parameter_set) const;
	void add_parameter(std::size_t index, nullable_field const & value);
	void recover_unwritten_parameters_below(std::size_t parameter_index, std::size_t last_active_row);
	void rebind_parameter_to_hold_value(std::size_t index, field const & value);

	bound_parameter_set & parameters_;
	std::size_t current_parameter_set_;
};

}
