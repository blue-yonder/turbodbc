#include "turbodbc/column.h"

#include <gtest/gtest.h>
#include "mock_classes.h"

#include <turbodbc/descriptions/string_description.h>
#include <boost/variant/get.hpp>


namespace {

	/**
	* Change the address of the given target_pointer to point to the third argument of the mocked function
	*/
	ACTION_P(store_pointer_to_buffer_in, target_pointer) {
		*target_pointer = &arg2;
	}

	std::size_t const column_index = 42;

}

TEST(ColumnTest, GetInfo)
{
	std::unique_ptr<turbodbc::string_description> description(new turbodbc::string_description("custom_name", false, 128));

	turbodbc_test::mock_statement statement;
	turbodbc::column column(statement, 0, 10, std::move(description));

	auto const info = column.get_info();
	EXPECT_EQ("custom_name", info.name);
	EXPECT_FALSE(info.supports_null_values);
	EXPECT_EQ(turbodbc::type_code::string, info.type);
	EXPECT_EQ(129, info.element_size);
}


TEST(ColumnTest, GetBuffer)
{
	std::unique_ptr<turbodbc::string_description> description(new turbodbc::string_description(128));

	turbodbc_test::mock_statement statement;

	turbodbc::column column(statement, column_index, 100, std::move(description));

	cpp_odbc::multi_value_buffer * buffer = nullptr;
	EXPECT_CALL(statement, do_bind_column(column_index, testing::_, testing::_))
		.WillOnce(store_pointer_to_buffer_in(&buffer));

	column.bind();
	EXPECT_EQ(buffer, &column.get_buffer());
}


TEST(ColumnTest, MoveConstructor)
{
	std::unique_ptr<turbodbc::string_description> description(new turbodbc::string_description(128));

	turbodbc_test::mock_statement statement;

	cpp_odbc::multi_value_buffer * buffer = nullptr;
	turbodbc::column moved(statement, column_index, 100, std::move(description));
	EXPECT_CALL(statement, do_bind_column(column_index, testing::_, testing::_))
		.WillOnce(store_pointer_to_buffer_in(&buffer));
	moved.bind();
	auto const expected_data_pointer = buffer->data_pointer();

	turbodbc::column column(std::move(moved));
	EXPECT_EQ(expected_data_pointer, column.get_buffer().data_pointer());
	EXPECT_EQ(nullptr, moved.get_buffer().data_pointer());

	EXPECT_CALL(statement, do_bind_column(column_index, testing::_, testing::_))
		.WillOnce(store_pointer_to_buffer_in(&buffer));
	ASSERT_NO_THROW(column.bind());
}
