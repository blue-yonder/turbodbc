#include <turbodbc_python/python_parameter_set.h>
#include <turbodbc_python/determine_parameter_type.h>
#include <turbodbc/parameter_sets/set_field.h>
#include <turbodbc/make_description.h>

#include <cpp_odbc/error.h>

#include <sqlext.h>

#include <sstream>
#include <ciso646>

namespace turbodbc {

python_parameter_set::python_parameter_set(bound_parameter_set & parameters) :
	parameters_(parameters),
	current_parameter_set_(0)
{
}

python_parameter_set::~python_parameter_set() = default;

void python_parameter_set::flush()
{
	// bound result set handles empty parameter sets
	parameters_.execute_batch(current_parameter_set_);
	current_parameter_set_ = 0;
}

void python_parameter_set::add_parameter_set(pybind11::iterable const & parameter_set)
{
	pybind11::iterable iterable(parameter_set);
	check_parameter_set(parameter_set);

	if (current_parameter_set_ == parameters_.buffered_sets()) {
		flush();
	}

	std::size_t parameter_index = 0;
	for (auto it = parameter_set.begin(); it != parameter_set.end(); ++it) {
		add_parameter(parameter_index, *it);
		++parameter_index;
	}

	++current_parameter_set_;
}

void python_parameter_set::check_parameter_set(pybind11::iterable const & parameter_set) const
{
	auto const expected_size = parameters_.number_of_parameters();
	std::size_t const actual_size = len(parameter_set);
	if (actual_size != expected_size) {
		std::ostringstream message;
		message << "Invalid number of parameters (expected " << expected_size
				<< ", got " << actual_size << ")";
		throw cpp_odbc::error(message.str());
	}
}

void python_parameter_set::add_parameter(std::size_t index, pybind11::handle const & value)
{
	if (not value.is_none()) {
		auto info = determine_parameter_type(value, parameters_.get_initial_parameter_types()[index]);

		if (parameters_.get_parameters()[index]->is_suitable_for(info.type, info.size)) {
			auto & parameter = *(parameters_.get_parameters()[index]);
			auto element = parameter.get_buffer()[current_parameter_set_];
			info.converter(value, element);
		} else {
			auto const last_active_set = current_parameter_set_;
			flush();
			recover_unwritten_parameters_below(index, last_active_set);
			parameters_.rebind(index, make_description(info.type, info.size));
			auto & parameter = *(parameters_.get_parameters()[index]);
			auto element = parameter.get_buffer()[0];
			info.converter(value, element);
		}
	} else {
		auto & parameter = *(parameters_.get_parameters()[index]);
		auto element = parameter.get_buffer()[current_parameter_set_];
		set_null(element);
	}
}

void python_parameter_set::recover_unwritten_parameters_below(std::size_t parameter_index, std::size_t last_active_row)
{
	for (std::size_t i = 0; i != parameter_index; ++i) {
		auto & parameter = *(parameters_.get_parameters()[i]);
		move_to_top(parameter, last_active_row);
	}
}

}
