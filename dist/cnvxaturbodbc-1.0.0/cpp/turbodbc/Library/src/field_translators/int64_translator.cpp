#include <turbodbc/field_translators/int64_translator.h>

#include <boost/variant/get.hpp>
#include <sqlext.h>

namespace turbodbc { namespace field_translators {

int64_translator::int64_translator() = default;

int64_translator::~int64_translator() = default;

field int64_translator::do_make_field(char const * data_pointer) const
{
	return {*reinterpret_cast<int64_t const *>(data_pointer)};
}

} }
