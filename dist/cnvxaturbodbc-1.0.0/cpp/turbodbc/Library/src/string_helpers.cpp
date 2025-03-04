#include <turbodbc/string_helpers.h>

#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sqlext.h>


namespace turbodbc {


std::size_t buffered_string_size(intptr_t length_indicator, std::size_t maximum_string_size)
{
    if (length_indicator == SQL_NO_TOTAL) {
        return maximum_string_size;
    } else {
        return std::min(static_cast<std::size_t>(length_indicator), maximum_string_size);
    }
}


}