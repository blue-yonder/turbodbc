#include "cpp_odbc/level3/raii_connection.h"

#include <gtest/gtest.h>

#include "cpp_odbc/level3/raii_statement.h"
#include "cpp_odbc/level3/raii_environment.h"
#include "cpp_odbc/error.h"
#include "cpp_odbc_test/level2_mock_api.h"

#include <type_traits>


using cpp_odbc::level3::raii_connection;
using cpp_odbc::level3::raii_environment;
using cpp_odbc::level3::raii_statement;
using cpp_odbc_test::level2_mock_api;
using cpp_odbc::level2::environment_handle;
using cpp_odbc::level2::connection_handle;
using cpp_odbc::level2::statement_handle;

namespace {

	// destinations for pointers, values irrelevant
	int value_a = 17;
	int value_b = 23;
	int value_c = 42;

	environment_handle const default_e_handle = {&value_a};
	connection_handle const default_c_handle = {&value_b};
	statement_handle const default_s_handle = {&value_c};


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

TEST(RaiiConnectionTest, IsConnection)
{
	bool const derived_from_connection = std::is_base_of<cpp_odbc::connection, raii_connection>::value;
	EXPECT_TRUE( derived_from_connection );
}


TEST(RaiiConnectionTest, RaiiConnectAndDisconnect)
{
	auto api = std::make_shared<testing::NiceMock<level2_mock_api> const>();
	ON_CALL(*api, do_allocate_environment_handle()).
			WillByDefault(testing::Return(default_e_handle));
	auto environment = std::make_shared<raii_environment>(api);
	connection_handle c_handle = {&value_a};

	std::string const connection_string = "my DSN";

	EXPECT_CALL(*api, do_allocate_connection_handle(default_e_handle)).
			WillOnce(testing::Return(c_handle));
	EXPECT_CALL(*api, do_establish_connection(c_handle, connection_string)).Times(1);

	{ // scope introduced for RAII check
		raii_connection connection(environment, connection_string);
		EXPECT_CALL(*api, do_end_transaction(c_handle, SQL_ROLLBACK)).Times(1); // automatic rollback
		EXPECT_CALL(*api, do_disconnect(c_handle)).Times(1);
		EXPECT_CALL(*api, do_free_handle(c_handle)).Times(1);
	}
}

TEST(RaiiConnectionTest, KeepsEnvironmentAlive)
{
	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);

	auto const use_count_before = environment.use_count();
	raii_connection connection(environment, "dummy");
	auto const use_count_after = environment.use_count();

	EXPECT_EQ(1, use_count_after - use_count_before);
}

TEST(RaiiConnectionTest, DestructorHandlesRollbackErrors)
{
	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);

	raii_connection connection(environment, "dummy");

	ON_CALL(*api, do_end_transaction(testing::_, SQL_ROLLBACK)).
		WillByDefault(testing::Throw(cpp_odbc::error("")));
}

TEST(RaiiConnectionTest, DestructorHandlesDisconnectErrors)
{
	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);

	raii_connection connection(environment, "dummy");

	ON_CALL(*api, do_disconnect(testing::_)).
		WillByDefault(testing::Throw(cpp_odbc::error("")));
}

TEST(RaiiConnectionTest, DestructorHandlesHandleFreeErrors)
{
	auto api = make_default_api();
	connection_handle c_handle = {&value_a};

	EXPECT_CALL(*api, do_allocate_connection_handle(testing::_)).
		WillOnce(testing::Return(c_handle));
	auto environment = std::make_shared<raii_environment>(api);

	raii_connection connection(environment, "dummy");

	ON_CALL(*api, do_free_handle(c_handle)).
		WillByDefault(testing::Throw(cpp_odbc::error("")));
}

TEST(RaiiConnectionTest, GetAPI)
{
	auto expected_api = make_default_api();
	auto environment = std::make_shared<raii_environment>(expected_api);

	raii_connection instance(environment, "dummy");
	EXPECT_TRUE( expected_api == instance.get_api());
}

TEST(RaiiConnectionTest, GetHandle)
{
	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);

	std::string const connection_string = "my DSN";
	raii_connection instance(environment, connection_string);

	bool const returns_handle_ref = std::is_same<connection_handle const &, decltype(instance.get_handle())>::value;

	EXPECT_TRUE( returns_handle_ref );
	instance.get_handle(); // make sure function symbol is there
}

TEST(RaiiConnectionTest, MakeStatement)
{
	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);

	auto connection = std::make_shared<raii_connection>(environment, "dummy");
	EXPECT_CALL(*api, do_allocate_statement_handle(default_c_handle))
		.WillOnce(testing::Return(default_s_handle));

	auto statement = connection->make_statement();
	bool const is_raii_statement = (std::dynamic_pointer_cast<raii_statement const>(statement) != nullptr);
	EXPECT_TRUE( is_raii_statement );
}

TEST(RaiiConnectionTest, SetAttribute)
{
	SQLINTEGER const attribute = 42;
	intptr_t const value = 1234;

	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);
	raii_connection connection(environment, "dummy");
	EXPECT_CALL(*api, do_set_connection_attribute(default_c_handle, attribute, value))
		.Times(1);

	connection.set_attribute(attribute, value);
}

TEST(RaiiConnectionTest, Commit)
{
	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);
	raii_connection connection(environment, "dummy");
	EXPECT_CALL(*api, do_end_transaction(default_c_handle, SQL_COMMIT))
		.Times(1);
	// implicit rollback at end of connection
	EXPECT_CALL(*api, do_end_transaction(default_c_handle, SQL_ROLLBACK));

	connection.commit();
}

TEST(RaiiConnectionTest, Rollback)
{
	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);
	raii_connection connection(environment, "dummy");
	EXPECT_CALL(*api, do_end_transaction(default_c_handle, SQL_ROLLBACK))
		.Times(1 + 1); // explicit rollback + implicit rollback at end of connection

	connection.rollback();
}

TEST(RaiiConnectionTest, GetStringInfo)
{
	SQLUSMALLINT const info_type = 42;
	std::string const expected("dummy");

	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);
	raii_connection connection(environment, "dummy");
	EXPECT_CALL(*api, do_get_string_connection_info(default_c_handle, info_type))
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, connection.get_string_info(info_type));
}

TEST(RaiiConnectionTest, GetIntegerInfo)
{
	SQLUSMALLINT const info_type = 42;
	SQLUINTEGER const expected = 23;

	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);
	raii_connection connection(environment, "dummy");
	EXPECT_CALL(*api, do_get_integer_connection_info(default_c_handle, info_type))
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, connection.get_integer_info(info_type));
}

TEST(RaiiConnectionTest, SupportsFunction)
{
	SQLUSMALLINT const function_id = 5;

	auto api = make_default_api();
	auto environment = std::make_shared<raii_environment>(api);
	raii_connection connection(environment, "dummy");
	EXPECT_CALL(*api, do_supports_function(default_c_handle, function_id))
		.WillOnce(testing::Return(false));

	EXPECT_FALSE(connection.supports_function(function_id));
}
