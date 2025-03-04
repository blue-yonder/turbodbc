#pragma once

#include <string>
#include <vector>

namespace cpp_odbc { namespace level2 {

/**
 * @brief This class serves as a converter from u16strings to a unixODBC compliant
 *        buffer.
 */
class input_u16string_buffer {
public:
	/**
	 * @brief Create a new buffer filled with the contents of the given u16string
	 * @param data The contents of this string are copied
	 */
	input_u16string_buffer(std::u16string const & data);

	/**
	 * @brief Retrieve the size of the buffered string
	 * @return The size of the buffered string
	 */
	std::size_t size() const;

	/**
	 * @brief Exposes internal data structure. Pass the result of this function
	 *        to unixODBC functions requesting pointers.
	 * @return A pointer to the characters in the string
	 */
	unsigned short * data_pointer();
private:
	std::vector<unsigned short> data_;
};

} }
