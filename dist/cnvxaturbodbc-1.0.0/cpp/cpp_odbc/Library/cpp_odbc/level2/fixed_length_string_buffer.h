#pragma once
/**
 *  @file fixed_length_string_buffer.h
 *  @date 07.03.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate: 2014-11-28 11:59:59 +0100 (Fr, 28 Nov 2014) $
 *  $LastChangedBy: mkoenig $
 *  $LastChangedRevision: 21206 $
 *
 */

#include <string>
#include <array>

namespace cpp_odbc { namespace level2 {

/**
 * @brief This class represents a buffer for strings of fixed length for use with the unixodbc C API.
 */
template <std::size_t Capacity>
class fixed_length_string_buffer {
public:
	/**
	 * @brief Create a new buffer with fixed length
	 */
	fixed_length_string_buffer()
	{
	}

	/**
	 * @brief Retrieve the capacity of the buffer
	 */
	std::size_t capacity() const
	{
		return Capacity;
	}

	/**
	 * @brief Retrieve a pointer to the internal buffer suitable for passing to unixodbc API functions.
	 *        This buffer contains the actual string data. Do not exceed the fixed capacity!
	 */
	unsigned char * data_pointer()
	{
		return data_.data();
	}

	/**
	 * @brief Conversion operator. Retrieve the buffered data a string.
	 */
	operator std::string() const
	{
		char const * characters = reinterpret_cast<char const *>(data_.data());
		return std::string(characters, Capacity);
	}

private:
	std::array<unsigned char, Capacity + 1> data_;
};


} }
