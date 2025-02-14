#include <turbodbc/parameter_sets/field_parameter_set.h>
#include <turbodbc/parameter_sets/set_field.h>
#include <turbodbc/make_description.h>

#include <cpp_odbc/error.h>

#include <sqlext.h>

#include <sstream>


namespace turbodbc {


field_parameter_set::field_parameter_set(bound_parameter_set & parameters) :
	parameters_(parameters),
	current_parameter_set_(0)
{
}

field_parameter_set::~field_parameter_set() = default;

void field_parameter_set::flush()
{
	// bound result set handles empty parameter sets
	parameters_.execute_batch(current_parameter_set_);
	current_parameter_set_ = 0;
}

void field_parameter_set::add_parameter_set(std::vector<nullable_field> const & parameter_set)
{
	check_parameter_set(parameter_set);

	if (current_parameter_set_ == parameters_.buffered_sets()) {
		flush();
	}

	for (unsigned int p = 0; p != parameter_set.size(); ++p) {
		add_parameter(p, parameter_set[p]);
	}

	++current_parameter_set_;
}

void field_parameter_set::check_parameter_set(std::vector<nullable_field> const & parameter_set) const
{
	auto const expected_size = parameters_.number_of_parameters();
	if (parameter_set.size() != expected_size) {
		std::ostringstream message;
		message << "Invalid number of parameters (expected " << expected_size
				<< ", got " << parameter_set.size() << ")";
		throw cpp_odbc::error(message.str());
	}
}

void field_parameter_set::add_parameter(std::size_t index, nullable_field const & value)
{
	if (value) {
		if (parameter_is_suitable_for(*(parameters_.get_parameters()[index]), *value)) {
			auto & parameter = *(parameters_.get_parameters()[index]);
			auto element = parameter.get_buffer()[current_parameter_set_];
			set_field(*value, element);
		} else {
			auto const last_active_set = current_parameter_set_;
			flush();
			recover_unwritten_parameters_below(index, last_active_set);
			rebind_parameter_to_hold_value(index, *value);
			auto & parameter = *(parameters_.get_parameters()[index]);
			auto element = parameter.get_buffer()[0];
			set_field(*value, element);
		}
	} else {
		auto & parameter = *(parameters_.get_parameters()[index]);
		auto element = parameter.get_buffer()[current_parameter_set_];
		set_null(element);
	}
}

void field_parameter_set::recover_unwritten_parameters_below(std::size_t parameter_index, std::size_t last_active_row)
{
	for (std::size_t i = 0; i != parameter_index; ++i) {
		auto & parameter = *(parameters_.get_parameters()[i]);
		move_to_top(parameter, last_active_row);
	}
}

void field_parameter_set::rebind_parameter_to_hold_value(std::size_t index, field const & value)
{
	auto description = make_description(value);
	parameters_.rebind(index, std::move(description));
}

}
