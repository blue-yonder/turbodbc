/**
 *  @file multi_value_buffer.cpp
 *  @date 11.04.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate: 2014-11-28 11:59:59 +0100 (Fr, 28 Nov 2014) $
 *  $LastChangedBy: mkoenig $
 *  $LastChangedRevision: 21206 $
 *
 */

#include "cpp_odbc/multi_value_buffer.h"

#include <stdexcept>

namespace cpp_odbc {


multi_value_buffer::multi_value_buffer(std::size_t element_size, std::size_t number_of_elements) :
	element_size_(element_size),
	data_(element_size_ * number_of_elements, 0),
	indicators_(number_of_elements, 0)
{
	if (element_size == 0) {
		throw std::logic_error("Element size must not be 0");
	}
	if (number_of_elements == 0) {
		throw std::logic_error("Number of elements must not be 0");
	}
}

multi_value_buffer::multi_value_buffer(multi_value_buffer && other) :
	element_size_(other.element_size_),
	data_(std::move(other.data_)),
	indicators_(std::move(other.indicators_))
{
	other.element_size_ = 0;
}

std::size_t multi_value_buffer::capacity_per_element() const
{
	return element_size_;
}

std::size_t multi_value_buffer::number_of_elements() const
{
	return data_.size() / element_size_;
}

char * multi_value_buffer::data_pointer()
{
	return data_.data();
}

char const * multi_value_buffer::data_pointer() const
{
	return data_.data();
}

intptr_t * multi_value_buffer::indicator_pointer()
{
	return indicators_.data();
}

intptr_t const * multi_value_buffer::indicator_pointer() const
{
	return indicators_.data();
}

writable_buffer_element multi_value_buffer::operator[](std::size_t element_index)
{
	return {
		data_pointer() + (element_index * element_size_),
		indicators_[element_index]
	};
}

buffer_element multi_value_buffer::operator[](std::size_t element_index) const
{
	return {
		data_.data() + (element_index * element_size_),
		indicators_[element_index]
	};
}



}
