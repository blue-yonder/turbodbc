#include <turbodbc/errors.h>

#include <gtest/gtest.h>

TEST(ErrorsTest, InterfaceError)
{
    turbodbc::interface_error error("message");
    EXPECT_EQ("message", std::string(error.what()));
}
