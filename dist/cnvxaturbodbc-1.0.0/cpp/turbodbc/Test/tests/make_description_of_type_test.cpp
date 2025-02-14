#include <turbodbc/make_description.h>

#include <gtest/gtest.h>
#include <turbodbc/descriptions.h>

#include <sqlext.h>
#include <sstream>
#include <stdexcept>


using turbodbc::make_description;
using turbodbc::field;
using turbodbc::type_code;

namespace {

	std::size_t const size_not_important = 0;

}

TEST(MakeDescriptionOfTypeTest, FromInteger)
{
	auto description = make_description(type_code::integer, size_not_important);
	ASSERT_TRUE( dynamic_cast<turbodbc::integer_description const *>(description.get()) );
}

TEST(MakeDescriptionOfTypeTest, FromDouble)
{
	auto description = make_description(type_code::floating_point, size_not_important);
	ASSERT_TRUE( dynamic_cast<turbodbc::floating_point_description const *>(description.get()) );
}

TEST(MakeDescriptionOfTypeTest, FromBool)
{
	auto description = make_description(type_code::boolean, size_not_important);
	ASSERT_TRUE( dynamic_cast<turbodbc::boolean_description const *>(description.get()) );
}

TEST(MakeDescriptionOfTypeTest, FromDate)
{
	auto description = make_description(type_code::date, size_not_important);
	ASSERT_TRUE( dynamic_cast<turbodbc::date_description const *>(description.get()) );
}

TEST(MakeDescriptionOfTypeTest, FromTimestamp)
{
	auto description = make_description(type_code::timestamp, size_not_important);
	ASSERT_TRUE( dynamic_cast<turbodbc::timestamp_description const *>(description.get()) );
}

TEST(MakeDescriptionOfTypeTest, FromStringProvidesMinimumLength)
{
	std::size_t const small_size = 2;
	auto description = make_description(type_code::string, small_size);
	auto as_string_description = dynamic_cast<turbodbc::string_description const *>(description.get());
	ASSERT_TRUE( as_string_description != nullptr );

	std::size_t const minimum_length = 10;
	EXPECT_EQ(as_string_description->element_size(), minimum_length + 1);
}

TEST(MakeDescriptionOfTypeTest, FromStringProvidesExtraSpaceForLargeStrings)
{
	std::string large_string("this is a relatively large string");
	auto description = make_description(type_code::string, large_string.size());
	auto as_string_description = dynamic_cast<turbodbc::string_description const *>(description.get());
	ASSERT_TRUE( as_string_description != nullptr );

	EXPECT_GT(as_string_description->element_size(), (large_string.size() + 1));
}

TEST(MakeDescriptionOfTypeTest, FromUnicodeProvidesMinimumLength)
{
	std::size_t const small_size = 2;
	auto description = make_description(type_code::unicode, small_size);
	auto as_unicode_description = dynamic_cast<turbodbc::unicode_description const *>(description.get());
	ASSERT_TRUE( as_unicode_description != nullptr );

	std::size_t const minimum_length = 10;
	EXPECT_EQ(as_unicode_description->element_size(), 2 * (minimum_length + 1));
}

TEST(MakeDescriptionOfTypeTest, FromUnicodeProvidesExtraSpaceForLargeStrings)
{
	std::string large_string("this is a relatively large string");
	auto description = make_description(type_code::unicode, large_string.size());
	auto as_unicode_description = dynamic_cast<turbodbc::unicode_description const *>(description.get());
	ASSERT_TRUE( as_unicode_description != nullptr );

	EXPECT_GT(as_unicode_description->element_size(), 2 * (large_string.size() + 1));
}
