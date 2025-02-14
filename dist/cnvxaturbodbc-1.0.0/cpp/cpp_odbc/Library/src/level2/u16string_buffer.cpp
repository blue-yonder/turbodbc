#include "cpp_odbc/level2/u16string_buffer.h"

namespace cpp_odbc { namespace level2 {

u16string_buffer::u16string_buffer(signed short int capacity) :
    data_(capacity),
    used_size_(0)
{
}

signed short int u16string_buffer::capacity() const
{
    return data_.capacity();
}

SQLWCHAR * u16string_buffer::data_pointer()
{
    return data_.data();
}

signed short int * u16string_buffer::size_pointer()
{
    return &used_size_;
}

u16string_buffer::operator std::u16string() const
{
    auto characters = reinterpret_cast<char16_t const *>(data_.data());
    return std::u16string(characters, used_size_);
}



} }
