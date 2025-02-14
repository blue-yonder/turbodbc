#include "cpp_odbc/credentials.h"

#include <gtest/gtest.h>

TEST(CredentialsTest, Members)
{
	std::string const user("username");
	std::string const pw("secret password");
	cpp_odbc::credentials creds = {user, pw};

	EXPECT_EQ(user, creds.user);
	EXPECT_EQ(pw, creds.password);
}
