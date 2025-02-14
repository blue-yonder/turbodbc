#pragma once

#include <turbodbc/field_translator.h>

namespace turbodbc { namespace field_translators {

/**
 * @brief Translates dates into buffer elements and vice versa
 */
class date_translator : public field_translator {
public:
	date_translator();
	~date_translator();
private:
	field do_make_field(char const * data_pointer) const final;
};

} }
