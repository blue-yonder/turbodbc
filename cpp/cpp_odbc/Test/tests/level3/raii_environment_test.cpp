#include "cpp_odbc/level3/raii_environment.h"

#include <gtest/gtest.h>

#include "cpp_odbc/level3/raii_connection.h"
#include "cpp_odbc/error.h"
#include "cpp_odbc_test/level2_mock_api.h"

#include "sqlext.h"

#include <type_traits>


using cpp_odbc::level3::raii_connection;
using cpp_odbc::level3::raii_environment;
using cpp_odbc_test::level2_mock_api;
using cpp_odbc::level2::environment_handle;
using cpp_odbc::level2::connection_handle;

namespace {
	// destinations for pointers, values irrelevant
	int value_a = 17;
	int value_b = 23;

	environment_handle const default_e_handle = {&value_a};
	connection_handle const default_c_handle = {&value_b};

	std::shared_ptr<testing::NiceMock<level2_mock_api> const> make_default_api()
	{
		auto api = std::make_shared<testing::NiceMock<level2_mock_api> const>();

		ON_CALL(*api, do_allocate_environment_handle())
			.WillByDefault(testing::Return(default_e_handle));
		ON_CALL(*api, do_allocate_connection_handle(testing::_))
			.WillByDefault(testing::Return(default_c_handle));

		return api;
	}

}

TEST(RaiiEnvironmentTest, IsEnvironment)
{
	bool const derived_from_environment = std::is_base_of<cpp_odbc::environment, raii_environment>::value;
	EXPECT_TRUE( derived_from_environment );
}

TEST(RaiiEnvironmentTest, ResourceManagement)
{
	environment_handle internal_handle = {&value_a};

	auto api = std::make_shared<testing::NiceMock<cpp_odbc_test::level2_mock_api> const>();

	EXPECT_CALL(*api, do_allocate_environment_handle()).
			WillOnce(testing::Return(internal_handle));

	// check that free_handle is called on destruction
	{
		raii_environment instance(api);

		EXPECT_CALL(*api, do_free_handle(internal_handle)).
				Times(1);
	}
}

TEST(RaiiEnvironmentTest, DestructorHandlesFreeHandleErrors)
{
	environment_handle internal_handle = {&value_a};

	auto api = std::make_shared<testing::NiceMock<cpp_odbc_test::level2_mock_api> const>();

	EXPECT_CALL(*api, do_allocate_environment_handle()).
		WillOnce(testing::Return(internal_handle));

	raii_environment instance(api);
	ON_CALL(*api, do_free_handle(internal_handle)).
		WillByDefault(testing::Throw(cpp_odbc::error("")));
}

TEST(RaiiEnvironmentTest, SetsODBCVersionOnConstruction)
{
	auto api = make_default_api();
	EXPECT_CALL(*api, do_set_environment_attribute(default_e_handle, SQL_ATTR_ODBC_VERSION, SQL_OV_ODBC3)).
			Times(1);

	raii_environment instance(api);
}

TEST(RaiiEnvironmentTest, GetAPI)
{
	auto expected_api = make_default_api();

	raii_environment instance(expected_api);
	EXPECT_TRUE( expected_api == instance.get_api());
}

TEST(RaiiEnvironmentTest, GetHandle)
{
	auto api = make_default_api();

	raii_environment instance(api);
	bool const returns_handle_ref = std::is_same<environment_handle const &, decltype(instance.get_handle())>::value;

	EXPECT_TRUE( returns_handle_ref );
	instance.get_handle(); // make sure function symbol is there
}

TEST(RaiiEnvironmentTest, MakeConnection)
{
	std::string const connection_string("dummy connection string");
	auto api = make_default_api();

	auto environment = std::make_shared<raii_environment>(api);
	EXPECT_CALL(*api, do_allocate_connection_handle(default_e_handle))
		.WillOnce(testing::Return(default_c_handle));
	EXPECT_CALL(*api, do_establish_connection(testing::_, connection_string))
		.Times(1);

	auto connection = environment->make_connection(connection_string);
	bool const is_raii_connection = (std::dynamic_pointer_cast<raii_connection const>(connection) != nullptr);
	EXPECT_TRUE( is_raii_connection );
}

TEST(RaiiEnvironmentTest, SetAttribute)
{
	SQLINTEGER const attribute = 42;
	intptr_t const value = 1234;

	auto api = make_default_api();
	raii_environment environment(api);

	EXPECT_CALL(*api, do_set_environment_attribute(default_e_handle, attribute, value))
		.Times(1);

	environment.set_attribute(attribute, value);
}
