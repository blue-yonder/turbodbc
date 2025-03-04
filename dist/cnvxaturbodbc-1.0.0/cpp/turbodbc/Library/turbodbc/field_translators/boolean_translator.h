#pragma once

#include <turbodbc/field_translator.h>

namespace turbodbc { namespace field_translators {

/**
 * @brief Translates boolean values into buffer elements and vice versa
 */
class boolean_translator : public field_translator {
public:
	boolean_translator();
	~boolean_translator();
private:
	field do_make_field(char const * data_pointer) const final;
};

} }
