#include <turbodbc/make_field_translator.h>

#include <turbodbc/field_translators.h>

#include <stdexcept>


using namespace turbodbc::field_translators;

namespace turbodbc {

std::unique_ptr<field_translator const> make_field_translator(turbodbc::column_info const & source)
{
	switch (source.type) {
		case type_code::boolean:
			return std::unique_ptr<boolean_translator>(new boolean_translator());
		case type_code::date:
			return std::unique_ptr<date_translator>(new date_translator());
		case type_code::floating_point:
			return std::unique_ptr<float64_translator>(new float64_translator());
		case type_code::integer:
			return std::unique_ptr<int64_translator>(new int64_translator());
		case type_code::string:
			return std::unique_ptr<string_translator>(new string_translator());
		case type_code::timestamp:
			return std::unique_ptr<timestamp_translator>(new timestamp_translator());
		default:
			throw std::logic_error("Unsupported type code!");
	}
}

}
