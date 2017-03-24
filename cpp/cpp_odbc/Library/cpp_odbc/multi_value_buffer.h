#pragma once

#include <vector>
#include <cstdint>

namespace cpp_odbc {

struct writable_buffer_element {
	char * data_pointer;
	intptr_t & indicator;
};

struct buffer_element {
	char const * data_pointer;
	intptr_t const & indicator;
};

/**
 * @brief This class serves as a buffer for multiple binary values. It provides access to its internals
 *        in order to be used for calls to SQLBindCol() and SQLBindParameter().
 */
class multi_value_buffer {
public:
	/**
	 * @brief Allocate internal data structures suitable to hold up to number_of_elements elements,
	 *        where each element may take up to element_size bytes of memory.
	 * @param element_size The number of bytes to reserve for each element.
	 *        Must be larger than zero.
	 * @param number_of_elements The number of elements in the array
	 *        Must be larger than zero.
	 */
	multi_value_buffer(std::size_t element_size, std::size_t number_of_elements);

	/**
	 * @brief Move constructor. Will leave the old buffer empty.
	 */
	multi_value_buffer(multi_value_buffer &&);

	/**
	 * @brief Retrieve the number of bytes reserved for each element
	 * @return The number of bytes
	 */
	std::size_t capacity_per_element() const;

	/**
	 * @brief Retrieve the number of elements that can be stored
	 *        in this buffer
	 * @return The number of elements
	 */
	std::size_t number_of_elements() const;

	/**
	 * @brief Provides raw access to a contiguous strip of memory which holds the actual
	 *        data. Intended to be used where TargetValuePtrs appear in ODBC calls.
	 * @return A pointer to the data array
	 */
	char * data_pointer();

	/**
	 * @brief Provides read-only raw access to a contiguous strip of memory
	 *        which holds the actual data.
	 * @return A pointer to the data array
	 */
	char const * data_pointer() const;

	/**
	 * @brief Provides raw access to a contiguous strip of memory which holds the length/indicator
	 *        information (for example, that a value is NULL). Intended to be used where
	 *        StrLen_or_IndPtrs appear in ODBC calls
	 * @return A pointer to the length/indicator array
	 */
	intptr_t * indicator_pointer();

	/**
	 * @brief Provides read-only raw access to a contiguous strip of memory which
	 *        holds the length/indicator information (for example, that a value
	 *        is NULL).
	 * @return A pointer to the length/indicator array
	 */
	intptr_t const * indicator_pointer() const;

	/**
	 * @brief Read/write access to the element with index element_index
	 * @param element_index The zero-based index of the element
	 * @return A read/write view on this element
	 */
	writable_buffer_element operator[](std::size_t element_index);

	/**
	 * @brief Read-only access to the element with index element_index
	 * @param element_index The zero-based index of the element
	 * @return A read-only view on this element
	 */
	buffer_element operator[](std::size_t element_index) const;

private:
	std::size_t element_size_;
	std::vector<char> data_;
	std::vector<intptr_t> indicators_;
};


}
