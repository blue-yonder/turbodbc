#include <turbodbc/field_translator.h>

#include <sql.h>

namespace turbodbc {

field_translator::field_translator() = default;

field_translator::~field_translator() = default;

nullable_field field_translator::make_field(cpp_odbc::buffer_element const & element) const
{
	if (element.indicator == SQL_NULL_DATA) {
		return {};
	} else {
		return do_make_field(element.data_pointer);
	}
}

}
