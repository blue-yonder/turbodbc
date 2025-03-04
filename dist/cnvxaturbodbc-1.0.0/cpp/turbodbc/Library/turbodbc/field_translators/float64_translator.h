#pragma once

#include <turbodbc/field_translator.h>

namespace turbodbc { namespace field_translators {

/**
 * @brief Translates 64 bit floating point values into buffer elements and vice versa
 */
class float64_translator : public field_translator {
public:
	float64_translator();
	~float64_translator();
private:
	field do_make_field(char const * data_pointer) const final;
};

} }
