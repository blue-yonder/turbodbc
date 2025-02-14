#include "turbodbc/parameter_sets/bound_parameter_set.h"

#include "turbodbc/make_description.h"
#include <cpp_odbc/error.h>

#include <gtest/gtest.h>
#include <tests/mock_classes.h>

#include <stdexcept>
#include <sqlext.h>


using namespace turbodbc;
typedef turbodbc_test::mock_statement mock_statement;


namespace {

    turbodbc::configuration make_config(std::size_t buffered_sets,
                                        bool prefer_unicode,
                                        bool query_db_for_initial_types)
    {
        turbodbc::capabilities capabilities(query_db_for_initial_types);
        turbodbc::options options;
        options.prefer_unicode = prefer_unicode;
        options.parameter_sets_to_buffer = buffered_sets;
        return turbodbc::configuration(options, capabilities);
    }

    bool const query_db_for_types = true;
    bool const do_not_query_db_for_types = false;
    bool const prefer_string = false;
    bool const prefer_unicode = true;

    cpp_odbc::column_description const int_description = {"dummy", SQL_BIGINT, 0, 0, true};
    cpp_odbc::column_description const timestamp_description = {"dummy", SQL_TYPE_TIMESTAMP, 0, 0, true};
    cpp_odbc::column_description const string_description_short = {"dummy", SQL_VARCHAR, 7, 0, true};
    cpp_odbc::column_description const string_description_max_length = {"dummy", SQL_VARCHAR, 16, 0, true};
    cpp_odbc::column_description const string_description_slightly_too_long = {"dummy", SQL_VARCHAR, 17, 0, true};
    cpp_odbc::column_description const string_description_too_long = {"dummy", SQL_VARCHAR, 50, 0, true};

}


TEST(BoundParameterSetTest, ConstructorBindsParametersBasedOnDBSuggestion)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(2));
    ON_CALL(statement, do_describe_parameter(1))
        .WillByDefault(testing::Return(int_description));
    ON_CALL(statement, do_describe_parameter(2))
        .WillByDefault(testing::Return(timestamp_description));

    EXPECT_CALL(statement, do_bind_input_parameter(1, SQL_C_SBIGINT, SQL_BIGINT, 0, testing::_)).Times(1);
    EXPECT_CALL(statement, do_bind_input_parameter(2, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, 6, testing::_)).Times(1);

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));
    EXPECT_EQ(params.number_of_parameters(), 2);
    EXPECT_EQ(params.get_parameters()[0]->get_buffer().number_of_elements(), 42);
    EXPECT_EQ(params.buffered_sets(), 42);
}


TEST(BoundParameterSetTest, ConstructorFallsBackToStringIfDBSuggestionFails)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(1));
    ON_CALL(statement, do_describe_parameter(1))
        .WillByDefault(testing::Throw(cpp_odbc::error("")));

    EXPECT_CALL(statement, do_bind_input_parameter(1, SQL_C_CHAR, SQL_VARCHAR, testing::_, testing::_)).Times(1);

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));
}


TEST(BoundParameterSetTest, ConstructorFallsBackToUnicodeIfDBSuggestionFails)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(1));
    ON_CALL(statement, do_describe_parameter(1))
        .WillByDefault(testing::Throw(cpp_odbc::error("")));

    EXPECT_CALL(statement, do_bind_input_parameter(1, SQL_C_WCHAR, SQL_WVARCHAR, testing::_, testing::_)).Times(1);

    bound_parameter_set params(statement, make_config(42, prefer_unicode, query_db_for_types));
}


TEST(BoundParameterSetTest, ConstructorBindsStringParametersByDefault)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(1));
    EXPECT_CALL(statement, do_describe_parameter(1)).Times(0);

    EXPECT_CALL(statement, do_bind_input_parameter(1, SQL_C_CHAR, SQL_VARCHAR, testing::_, testing::_)).Times(1);

    bound_parameter_set params(statement, make_config(42, prefer_string, do_not_query_db_for_types));
}


TEST(BoundParameterSetTest, ConstructorBindsUnicodeParametersByDefault)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(1));
    EXPECT_CALL(statement, do_describe_parameter(1)).Times(0);

    EXPECT_CALL(statement, do_bind_input_parameter(1, SQL_C_WCHAR, SQL_WVARCHAR, testing::_, testing::_)).Times(1);

    bound_parameter_set params(statement, make_config(42, prefer_unicode, do_not_query_db_for_types));
}


TEST(BoundParameterSetTest, ConstructorBindsUnicodeWhenPreferred)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(1));
    ON_CALL(statement, do_describe_parameter(1))
        .WillByDefault(testing::Return(string_description_short));

    EXPECT_CALL(statement, do_bind_input_parameter(1, SQL_C_WCHAR, SQL_WVARCHAR, testing::_, testing::_)).Times(1);

    bound_parameter_set params(statement, make_config(42, prefer_unicode, query_db_for_types));
}


TEST(BoundParameterSetTest, ConstructorOverridesStringParameterLengthSuggestions)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(4));
    ON_CALL(statement, do_describe_parameter(1))
        .WillByDefault(testing::Return(string_description_short));
    ON_CALL(statement, do_describe_parameter(2))
        .WillByDefault(testing::Return(string_description_max_length));
    ON_CALL(statement, do_describe_parameter(3))
        .WillByDefault(testing::Return(string_description_slightly_too_long));
    ON_CALL(statement, do_describe_parameter(4))
        .WillByDefault(testing::Return(string_description_too_long));

    EXPECT_CALL(statement, do_bind_input_parameter(1, SQL_C_CHAR, SQL_VARCHAR, testing::_, testing::_)).Times(1);
    EXPECT_CALL(statement, do_bind_input_parameter(2, SQL_C_CHAR, SQL_VARCHAR, testing::_, testing::_)).Times(1);
    EXPECT_CALL(statement, do_bind_input_parameter(3, SQL_C_CHAR, SQL_VARCHAR, testing::_, testing::_)).Times(1);
    EXPECT_CALL(statement, do_bind_input_parameter(4, SQL_C_CHAR, SQL_VARCHAR, testing::_, testing::_)).Times(1);

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));
    EXPECT_EQ(params.get_parameters()[0]->get_buffer().capacity_per_element(), string_description_short.size + 1);
    EXPECT_EQ(params.get_parameters()[1]->get_buffer().capacity_per_element(), string_description_max_length.size + 1);
    EXPECT_EQ(params.get_parameters()[2]->get_buffer().capacity_per_element(), string_description_max_length.size + 1);
    EXPECT_EQ(params.get_parameters()[3]->get_buffer().capacity_per_element(), string_description_max_length.size + 1);
}


TEST(BoundParameterSetTest, ConstructorOverridesUnicodeParameterLengthSuggestions)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(4));
    ON_CALL(statement, do_describe_parameter(1))
        .WillByDefault(testing::Return(string_description_short));
    ON_CALL(statement, do_describe_parameter(2))
        .WillByDefault(testing::Return(string_description_max_length));
    ON_CALL(statement, do_describe_parameter(3))
        .WillByDefault(testing::Return(string_description_slightly_too_long));
    ON_CALL(statement, do_describe_parameter(4))
        .WillByDefault(testing::Return(string_description_too_long));

    EXPECT_CALL(statement, do_bind_input_parameter(1, SQL_C_WCHAR, SQL_WVARCHAR, testing::_, testing::_)).Times(1);
    EXPECT_CALL(statement, do_bind_input_parameter(2, SQL_C_WCHAR, SQL_WVARCHAR, testing::_, testing::_)).Times(1);
    EXPECT_CALL(statement, do_bind_input_parameter(3, SQL_C_WCHAR, SQL_WVARCHAR, testing::_, testing::_)).Times(1);
    EXPECT_CALL(statement, do_bind_input_parameter(4, SQL_C_WCHAR, SQL_WVARCHAR, testing::_, testing::_)).Times(1);

    bound_parameter_set params(statement, make_config(42, prefer_unicode, query_db_for_types));
    EXPECT_EQ(params.get_parameters()[0]->get_buffer().capacity_per_element(), 2 * (string_description_short.size + 1));
    EXPECT_EQ(params.get_parameters()[1]->get_buffer().capacity_per_element(), 2 * (string_description_max_length.size + 1));
    EXPECT_EQ(params.get_parameters()[2]->get_buffer().capacity_per_element(), 2 * (string_description_max_length.size + 1));
    EXPECT_EQ(params.get_parameters()[3]->get_buffer().capacity_per_element(), 2 * (string_description_max_length.size + 1));
}


TEST(BoundParameterSetTest, Rebind)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(2));
    ON_CALL(statement, do_describe_parameter(1))
        .WillByDefault(testing::Return(int_description));
    ON_CALL(statement, do_describe_parameter(2))
        .WillByDefault(testing::Return(timestamp_description));

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));

    std::size_t const column_index = 1;
    auto const one_based_column_index = column_index + 1;
    EXPECT_CALL(statement, do_bind_input_parameter(one_based_column_index, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, 6, testing::_))
        .Times(1);

    params.rebind(1, make_description(type_code::timestamp, 0));
    EXPECT_EQ(params.get_parameters()[1]->get_buffer().capacity_per_element(), sizeof(SQL_TIMESTAMP_STRUCT));
    EXPECT_EQ(params.get_parameters()[1]->get_buffer().number_of_elements(), 42);
}

namespace {
    template <typename MockStatement>
    void configure_single_param(MockStatement & statement) {
        ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(1));
        ON_CALL(statement, do_describe_parameter(1))
                .WillByDefault(testing::Return(int_description));
    }
}


TEST(BoundParameterSetTest, GetInitialParameterTypes)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(2));
    ON_CALL(statement, do_describe_parameter(1))
        .WillByDefault(testing::Return(int_description));
    ON_CALL(statement, do_describe_parameter(2))
        .WillByDefault(testing::Return(timestamp_description));

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));

    std::vector<type_code> const expected = {type_code::integer, type_code::timestamp};
    EXPECT_EQ(params.get_initial_parameter_types(), expected);
}


TEST(BoundParameterSetTest, ExecuteBatchNoSets)
{
    mock_statement statement;
    configure_single_param(statement);

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));

    EXPECT_CALL(statement, do_execute_prepared()).Times(0);

    params.execute_batch(0);
}

TEST(BoundParameterSetTest, ExecuteBatchNoParameters)
{
    mock_statement statement;
    ON_CALL(statement, do_number_of_parameters()).WillByDefault(testing::Return(0));

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));

    EXPECT_CALL(statement, do_execute_prepared()).Times(0);

    params.execute_batch(23);
}


TEST(BoundParameterSetTest, ExecuteBatchThrowsIfBatchTooLarge)
{
    mock_statement statement;
    configure_single_param(statement);

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));

    ASSERT_THROW(params.execute_batch(43), std::logic_error);
}


TEST(BoundParameterSetTest, ExecuteBatch)
{
    mock_statement statement;
    configure_single_param(statement);

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));

    testing::InSequence ordered;
    EXPECT_CALL(statement, do_set_attribute(SQL_ATTR_PARAMSET_SIZE, 23));
    EXPECT_CALL(statement, do_execute_prepared());

    params.execute_batch(23);
}


namespace {

    struct fake_statement : public turbodbc_test::default_mock_statement {
        fake_statement() :
                input(0),
                processed_ptr(nullptr),
                process_parameters(true)
        {}

        void do_set_attribute(SQLINTEGER attribute, intptr_t value) const final
        {
            if (attribute == SQL_ATTR_PARAMSET_SIZE) {
                input = value;
            }
        }

        void do_set_attribute(SQLINTEGER attribute, SQLULEN * pointer) const final
        {
            if (attribute == SQL_ATTR_PARAMS_PROCESSED_PTR) {
                processed_ptr = pointer;
            }
        }

        void do_execute_prepared() const final
        {
            if (process_parameters) {
                *processed_ptr = input;
            } else {
                *processed_ptr = 0;
            }
        }

        mutable std::size_t input;
        mutable SQLULEN * processed_ptr;
        bool process_parameters;
    };

}


TEST(BoundParameterSetTest, TransferredSetsRespectsDatabaseFeedback)
{
    testing::NiceMock<fake_statement> statement;
    configure_single_param(statement);

    bound_parameter_set params(statement, make_config(42, prefer_string, query_db_for_types));

    EXPECT_EQ(params.transferred_sets(), 0);
    params.execute_batch(17);
    EXPECT_EQ(params.transferred_sets(), 17);
    params.execute_batch(29);
    EXPECT_EQ(params.transferred_sets(), 46);

    statement.process_parameters = false;
    params.execute_batch(23);
    EXPECT_EQ(params.transferred_sets(), 46);
}
