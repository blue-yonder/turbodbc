#include "cpp_odbc/environment.h"

#include <gtest/gtest.h>

#include "cpp_odbc_test/mock_connection.h"
#include "cpp_odbc_test/mock_environment.h"


using cpp_odbc_test::mock_environment;

TEST(EnvironmentTest, IsSharable)
{
	auto environment = std::make_shared<mock_environment>();
	auto shared = environment->shared_from_this();
	EXPECT_TRUE( environment == shared );
}

TEST(EnvironmentTest, MakeConnectionForwards)
{
	mock_environment environment;
	std::string const connection_string("test DSN");
	auto expected = std::make_shared<cpp_odbc_test::mock_connection>();

	EXPECT_CALL(environment, do_make_connection(connection_string))
		.WillOnce(testing::Return(expected));

	EXPECT_TRUE( expected == environment.make_connection(connection_string) );
}

TEST(EnvironmentTest, SetIntegerAttributeForwards)
{
	mock_environment environment;
	SQLINTEGER const attribute = 42;
	intptr_t const value = 17;

	EXPECT_CALL(environment, do_set_attribute(attribute, value)).Times(1);

	environment.set_attribute(attribute, value);
}
