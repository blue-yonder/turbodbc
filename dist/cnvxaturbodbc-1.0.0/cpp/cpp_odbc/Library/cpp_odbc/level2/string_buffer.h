#pragma once

#include <string>
#include <vector>

namespace cpp_odbc { namespace level2 {

/**
 * @brief This class represents a buffer for strings for use with the unixodbc C API.
 */
class string_buffer {
public:
    /**
     * @brief Constructs a new string buffer with the given capacity, i.e., maximum size
     * @param capacity Capacity of the buffer
     */
    string_buffer(signed short int capacity);

    /**
     * @brief Retrieve the capacity of the buffer in a format suitable for passing
     *        to unixodbc API functions.
     */
    signed short int capacity() const;

    /**
     * @brief Retrieve a pointer to the internal buffer suitable for passing to unixodbc API functions.
     *        This buffer contains the actual string data. Do not exceed the allocated capacity!
     */
    unsigned char * data_pointer();

    /**
     * @brief Retrieve a pointer to a size buffer suitable for passing to unixodbc API functions.
     *        This buffer contains the number of significant bytes in the buffer returned by
     *        data_pointer().
     */
    signed short int * size_pointer();

    /**
     * @brief Conversion operator. Retrieve the buffered data as a string. Bad things will happen if
     *        the value of size_pointer is larger than the capacity!
     */
    operator std::string() const;

private:
    std::vector<unsigned char> data_;
    signed short int used_size_;
};


} }
