#include <cstdint>
#include <cstring>

namespace turbodbc {

std::size_t buffered_string_size(intptr_t length_indicator, std::size_t maximum_string_size);

}