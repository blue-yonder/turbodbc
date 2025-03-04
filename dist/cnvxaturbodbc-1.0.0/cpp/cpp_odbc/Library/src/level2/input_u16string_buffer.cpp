#include "cpp_odbc/level2/input_u16string_buffer.h"

#include <cstring>

namespace cpp_odbc { namespace level2 {

input_u16string_buffer::input_u16string_buffer(std::u16string const & data) :
	data_(data.size() + 1)
{
	memcpy(data_.data(), data.data(), data_.size() * sizeof(char16_t));
}

std::size_t input_u16string_buffer::size() const
{
	return data_.size() - 1;
}

unsigned short * input_u16string_buffer::data_pointer()
{
	return data_.data();
}

} }
