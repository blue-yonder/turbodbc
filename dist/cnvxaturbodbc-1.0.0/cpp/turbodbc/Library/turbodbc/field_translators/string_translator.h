#pragma once

#include <turbodbc/field_translator.h>

namespace turbodbc { namespace field_translators {

/**
 * @brief Translates string values into buffer elements and vice versa
 */
class string_translator : public field_translator {
public:
	string_translator();
	~string_translator();
private:
	field do_make_field(char const * data_pointer) const final;
};

} }
