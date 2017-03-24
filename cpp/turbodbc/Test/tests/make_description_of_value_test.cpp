#include <turbodbc/make_description.h>

#include <gtest/gtest.h>
#include <turbodbc/descriptions.h>

#include <sqlext.h>
#include <sstream>
#include <stdexcept>


using turbodbc::make_description;
using turbodbc::field;


TEST(MakeDescriptionOfValueTest, FromInteger)
{
	field const value(int64_t(42));
	auto description = make_description(value);
	ASSERT_TRUE( dynamic_cast<turbodbc::integer_description const *>(description.get()) );
}

TEST(MakeDescriptionOfValueTest, FromDouble)
{
	field const value(3.14);
	auto description = make_description(value);
	ASSERT_TRUE( dynamic_cast<turbodbc::floating_point_description const *>(description.get()) );
}

TEST(MakeDescriptionOfValueTest, FromBool)
{
	field const value(true);
	auto description = make_description(value);
	ASSERT_TRUE( dynamic_cast<turbodbc::boolean_description const *>(description.get()) );
}

TEST(MakeDescriptionOfValueTest, FromDate)
{
	field const value(boost::gregorian::date(2016, 1, 7));
	auto description = make_description(value);
	ASSERT_TRUE( dynamic_cast<turbodbc::date_description const *>(description.get()) );
}

TEST(MakeDescriptionOfValueTest, FromPtime)
{
	field const value(boost::posix_time::ptime({2016, 1, 7}, {1, 2, 3, 123456}));
	auto description = make_description(value);
	ASSERT_TRUE( dynamic_cast<turbodbc::timestamp_description const *>(description.get()) );
}

TEST(MakeDescriptionOfValueTest, FromStringProvidesMinimumLength)
{
	std::string small_string("hi");
	field const value(small_string);
	auto description = make_description(value);
	auto as_string_description = dynamic_cast<turbodbc::string_description const *>(description.get());
	ASSERT_TRUE( as_string_description != nullptr );

	std::size_t const minimum_length = 10;
	EXPECT_EQ(as_string_description->element_size(), minimum_length + 1);
}

TEST(MakeDescriptionOfValueTest, FromStringProvidesExtraSpaceForLargeStrings)
{
	std::string large_string("this is a relatively large string");
	field const value(large_string);
	auto description = make_description(value);
	auto as_string_description = dynamic_cast<turbodbc::string_description const *>(description.get());
	ASSERT_TRUE( as_string_description != nullptr );

	EXPECT_GT(as_string_description->element_size(), (large_string.size() + 1));
}
