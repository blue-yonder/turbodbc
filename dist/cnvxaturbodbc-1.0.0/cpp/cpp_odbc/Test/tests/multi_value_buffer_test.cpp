#include "cpp_odbc/multi_value_buffer.h"

#include <gtest/gtest.h>

#include <cstring>
#include <stdexcept>


using cpp_odbc::multi_value_buffer;

TEST(MultiValueBufferTest, ConstructorEnforcesPositiveParameters)
{
	EXPECT_THROW( multi_value_buffer(0, 1), std::logic_error );
	EXPECT_THROW( multi_value_buffer(1, 0), std::logic_error );
	EXPECT_NO_THROW( multi_value_buffer(1, 1) );
}

TEST(MultiValueBufferTest, CapacityPerElement)
{
	std::size_t const element_size = 100;
	multi_value_buffer buffer(element_size, 42);

	EXPECT_EQ( element_size, buffer.capacity_per_element() );
}

TEST(MultiValueBufferTest, NumberOfElements)
{
	std::size_t const n_elements = 42;
	multi_value_buffer buffer(100, n_elements);

	EXPECT_EQ(n_elements, buffer.number_of_elements());
}


TEST(MultiValueBufferTest, DataPointer)
{
	std::size_t const element_size = 100;
	std::size_t const number_of_elements = 42;
	multi_value_buffer buffer(element_size, number_of_elements);

	// write something to have some protection against segmentation faults
	std::memset( buffer.data_pointer(), 0xff, element_size * number_of_elements );
}

TEST(MultiValueBufferTest, ConstDataPointer)
{
	std::size_t const element_size = 100;
	std::size_t const number_of_elements = 42;
	multi_value_buffer buffer(element_size, number_of_elements);
	multi_value_buffer const & as_const(buffer);

	EXPECT_EQ(buffer.data_pointer(), as_const.data_pointer());
}

TEST(MultiValueBufferTest, IndicatorPointer)
{
	std::size_t const number_of_elements = 42;
	multi_value_buffer buffer(3, number_of_elements);

	// write something to have some protection against segmentation faults
	buffer.indicator_pointer()[number_of_elements - 1] = 17;
}

TEST(MultiValueBufferTest, ConstIndicatorPointer)
{
	std::size_t const number_of_elements = 42;
	multi_value_buffer buffer(3, number_of_elements);
	multi_value_buffer const & as_const(buffer);

	EXPECT_EQ(buffer.indicator_pointer(), as_const.indicator_pointer());
}

TEST(MultiValueBufferTest, MutableElementAccess)
{
	std::size_t const element_size = 3;
	std::size_t const number_of_elements = 2;
	multi_value_buffer buffer(element_size, number_of_elements);

	std::strcpy( buffer[0].data_pointer, "abc" );
	std::strcpy( buffer[1].data_pointer, "def" );
	EXPECT_EQ( 0, std::memcmp(buffer.data_pointer(), "abcdef", 6));

	int64_t const expected_indicator = 42;
	buffer[1].indicator = expected_indicator;
	EXPECT_EQ( expected_indicator, buffer.indicator_pointer()[1]);
}

TEST(MultiValueBufferTest, ConstElementAccess)
{
	std::size_t const element_size = 3;
	std::size_t const number_of_elements = 2;
	multi_value_buffer buffer(element_size, number_of_elements);

	std::string const data = "abcdef";
	int64_t const expected_indicator = 17;

	std::strcpy( buffer.data_pointer(), data.c_str() );
	buffer.indicator_pointer()[1] = expected_indicator;

	auto const & const_buffer = buffer;

	EXPECT_EQ( data[element_size], *const_buffer[1].data_pointer );
	EXPECT_EQ( expected_indicator, const_buffer[1].indicator );
}

TEST(MultiValueBufferTest, MoveConstructor)
{
	std::size_t const element_size = 100;
	std::size_t const number_of_elements = 42;
	multi_value_buffer moved(element_size, number_of_elements);
	multi_value_buffer buffer(std::move(moved));

	EXPECT_EQ(0, moved.capacity_per_element());
	EXPECT_EQ(nullptr, moved.data_pointer());
	EXPECT_EQ(nullptr, moved.indicator_pointer());

	EXPECT_EQ(element_size, buffer.capacity_per_element());
	auto element = buffer[number_of_elements - 1];
	// write something to be sure there is memory behind all this
	element.indicator = 42;
	std::strcpy(element.data_pointer, "abc");
}
