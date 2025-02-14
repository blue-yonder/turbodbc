#pragma once

#include <turbodbc/field_translator.h>

namespace turbodbc { namespace field_translators {

/**
 * @brief Translates timestamps into buffer elements and vice versa
 */
class timestamp_translator : public field_translator {
public:
	timestamp_translator();
	~timestamp_translator();
private:
	field do_make_field(char const * data_pointer) const final;
};

} }
