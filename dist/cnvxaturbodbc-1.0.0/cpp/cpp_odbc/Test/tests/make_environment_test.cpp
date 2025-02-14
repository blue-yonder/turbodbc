#include "cpp_odbc/make_environment.h"

#include <gtest/gtest.h>

#include "cpp_odbc/level3/raii_environment.h"
#include <memory>

TEST(MakeEnvironmentTest, ProductionEnvironment)
{
	auto environment = cpp_odbc::make_environment();
	bool const is_raii_environment =
			(std::dynamic_pointer_cast<cpp_odbc::level3::raii_environment>(environment) != nullptr);
	EXPECT_TRUE( is_raii_environment );
}

TEST(MakeEnvironmentTest, DebugEnvironment)
{
	auto environment = cpp_odbc::make_debug_environment();
	bool const is_raii_environment =
			(std::dynamic_pointer_cast<cpp_odbc::level3::raii_environment>(environment) != nullptr);
	EXPECT_TRUE( is_raii_environment );
}
