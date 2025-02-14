#include "cpp_odbc/level2/string_buffer.h"

namespace cpp_odbc { namespace level2 {

string_buffer::string_buffer(signed short int capacity) :
    data_(capacity),
    used_size_(0)
{
}

signed short int string_buffer::capacity() const
{
    return data_.capacity();
}

unsigned char * string_buffer::data_pointer()
{
    return data_.data();
}

signed short int * string_buffer::size_pointer()
{
    return &used_size_;
}

string_buffer::operator std::string() const
{
    auto characters = reinterpret_cast<char const *>(data_.data());
    return std::string(characters, used_size_);
}



} }
