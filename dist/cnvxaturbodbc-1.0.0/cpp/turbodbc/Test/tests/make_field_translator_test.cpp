#include <turbodbc/make_field_translator.h>
#include <turbodbc/field_translators.h>

#include <gtest/gtest.h>

#include <stdexcept>


using turbodbc::make_field_translator;

namespace {

	std::size_t const size_unimportant = 8;

}

TEST(MakeFieldTranslatorTest, UnsupportedTypeThrows)
{
	turbodbc::column_info info = {"name", turbodbc::type_code::integer, size_unimportant, true};
	info.type = static_cast<turbodbc::type_code>(-666); // assign invalid code
	EXPECT_THROW(make_field_translator(info), std::logic_error);
}

TEST(MakeFieldTranslatorTest, BooleanType)
{
	turbodbc::column_info info = {"name", turbodbc::type_code::boolean, size_unimportant, true};
	EXPECT_TRUE(dynamic_cast<turbodbc::field_translators::boolean_translator const *>(make_field_translator(info).get()));
}

TEST(MakeFieldTranslatorTest, DateType)
{
	turbodbc::column_info info = {"name", turbodbc::type_code::date, size_unimportant, true};
	EXPECT_TRUE(dynamic_cast<turbodbc::field_translators::date_translator const *>(make_field_translator(info).get()));
}

TEST(MakeFieldTranslatorTest, Float64Type)
{
	turbodbc::column_info info = {"name", turbodbc::type_code::floating_point, size_unimportant, true};
	EXPECT_TRUE(dynamic_cast<turbodbc::field_translators::float64_translator const *>(make_field_translator(info).get()));
}

TEST(MakeFieldTranslatorTest, Int64Type)
{
	turbodbc::column_info info = {"name", turbodbc::type_code::integer, size_unimportant, true};
	EXPECT_TRUE(dynamic_cast<turbodbc::field_translators::int64_translator const *>(make_field_translator(info).get()));
}

TEST(MakeFieldTranslatorTest, StringType)
{
	turbodbc::column_info info = {"name", turbodbc::type_code::string, size_unimportant, true};
	EXPECT_TRUE(dynamic_cast<turbodbc::field_translators::string_translator const *>(make_field_translator(info).get()));
}

TEST(MakeFieldTranslatorTest, TimestampType)
{
	turbodbc::column_info info = {"name", turbodbc::type_code::timestamp, size_unimportant, true};
	EXPECT_TRUE(dynamic_cast<turbodbc::field_translators::timestamp_translator const *>(make_field_translator(info).get()));
}
