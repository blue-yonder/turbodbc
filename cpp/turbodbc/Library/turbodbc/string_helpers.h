#include <cstdint>
#include <cstring>

namespace turbodbc {

/***
 * @brief Determine the number of bytes returned by odbc for string types, given a length indicator
 * @param length_indicator The length indicator, as returned by odbc. Must not be SQL_NULL_DATA, but may be SQL_NO_TOTAL
 * @param maximum_string_size The maximum string size in bytes, used if length_indicator is SQL_NO_TOTAL;
 *        should be set to buffer size in bytes minus the byte-size of the null termination character
 */
std::size_t buffered_string_size(intptr_t length_indicator, std::size_t maximum_string_size);

}