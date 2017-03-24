#include "turbodbc/parameter.h"

#include <gtest/gtest.h>
#include "mock_classes.h"

#include <turbodbc/descriptions/integer_description.h>
#include <turbodbc/descriptions/string_description.h>
#include <turbodbc/descriptions/unicode_description.h>
#include <boost/variant/get.hpp>

namespace {

	std::size_t const parameter_index = 42;

}


TEST(ParameterTest, GetTypeCode)
{
	std::unique_ptr<turbodbc::string_description> description(new turbodbc::string_description(10));

	turbodbc_test::mock_statement statement;

	turbodbc::parameter parameter(statement, parameter_index, 100, std::move(description));

	EXPECT_EQ(turbodbc::type_code::string, parameter.get_type_code());
}


TEST(ParameterTest, GetBuffer)
{
	std::unique_ptr<turbodbc::string_description> description(new turbodbc::string_description(10));

	turbodbc_test::mock_statement statement;

	auto const buffered_rows = 100;
	turbodbc::parameter parameter(statement, parameter_index, buffered_rows, std::move(description));

	auto & buffer = parameter.get_buffer();
	// test read/write access to last element is valid
	buffer[99].indicator = 42;
	EXPECT_EQ(42, buffer[99].indicator);
}


TEST(ParameterTest, MoveToTop)
{
	std::unique_ptr<turbodbc::string_description> description(new turbodbc::string_description(10));

	turbodbc_test::mock_statement statement;

	auto const buffered_rows = 100;
	turbodbc::parameter parameter(statement, parameter_index, buffered_rows, std::move(description));

	auto const row_index = 42;
	std::string const expected("hi there!");
	std::memcpy(parameter.get_buffer()[row_index].data_pointer, expected.c_str(), expected.size() + 1);
	parameter.get_buffer()[row_index].indicator = expected.size();

	move_to_top(parameter, row_index);

	auto const & buffer = parameter.get_buffer();

	std::string const top_content(buffer[0].data_pointer);
	auto const top_indicator(buffer[0].indicator);
	EXPECT_EQ(expected, top_content);
	EXPECT_EQ(expected.size(), top_indicator);
}


TEST(ParameterTest, IsSuitableForString)
{
	std::unique_ptr<turbodbc::string_description> description(new turbodbc::string_description(10));
	EXPECT_EQ(description->element_size(), 11); // 11 bytes wide

	turbodbc_test::mock_statement statement;

	turbodbc::parameter parameter(statement, parameter_index, 100, std::move(description));

	EXPECT_TRUE(parameter.is_suitable_for(turbodbc::type_code::string, 10)); // takes up 11 bytes inclusing null termination
	EXPECT_TRUE(parameter.is_suitable_for(turbodbc::type_code::string, 9));

	EXPECT_FALSE(parameter.is_suitable_for(turbodbc::type_code::string, 11)); // takes up 12 bytes including null termination
	EXPECT_FALSE(parameter.is_suitable_for(turbodbc::type_code::unicode, 1)); // just 4 bytes including null termination, but wrong type
}

TEST(ParameterTest, IsSuitableForUnicode)
{
	std::unique_ptr<turbodbc::unicode_description> description(new turbodbc::unicode_description(10));
	EXPECT_EQ(description->element_size(), 22); // 22 bytes wide

	turbodbc_test::mock_statement statement;

	turbodbc::parameter parameter(statement, parameter_index, 100, std::move(description));

	EXPECT_TRUE(parameter.is_suitable_for(turbodbc::type_code::unicode, 10)); // takes up 22 bytes inclusing null termination
	EXPECT_TRUE(parameter.is_suitable_for(turbodbc::type_code::unicode, 9));

	EXPECT_FALSE(parameter.is_suitable_for(turbodbc::type_code::unicode, 11)); // takes up 24 bytes including null termination
}


TEST(ParameterTest, IsSuitableForOther)
{
	std::unique_ptr<turbodbc::integer_description> description(new turbodbc::integer_description());
	auto const supported_size = description->element_size();

	turbodbc_test::mock_statement statement;

	turbodbc::parameter parameter(statement, parameter_index, 100, std::move(description));

	EXPECT_TRUE(parameter.is_suitable_for(turbodbc::type_code::integer, supported_size));
	EXPECT_FALSE(parameter.is_suitable_for(turbodbc::type_code::floating_point, supported_size));
}
