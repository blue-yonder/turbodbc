#include <turbodbc/field_translators/boolean_translator.h>

#include <boost/variant/get.hpp>


namespace turbodbc { namespace field_translators {

boolean_translator::boolean_translator() = default;

boolean_translator::~boolean_translator() = default;

field boolean_translator::do_make_field(char const * data_pointer) const
{
	return {static_cast<bool>(*data_pointer)};
}

} }
