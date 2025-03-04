#include <turbodbc/result_sets/double_buffered_result_set.h>

#include <tests/mock_classes.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <type_traits>
#include <algorithm>
#include <memory>

#include <sqlext.h>

using turbodbc::result_sets::double_buffered_result_set;
using turbodbc::column_info;
using turbodbc_test::mock_statement;


namespace {

    turbodbc::options make_options(turbodbc::buffer_size const & size, bool prefer_unicode) {
        turbodbc::options options;
        options.read_buffer_size = size;
        options.prefer_unicode = prefer_unicode;
        return options;
    }

    bool const prefer_string = false;
    bool const prefer_unicode = true;

    std::shared_ptr<mock_statement> prepare_mock_with_columns(std::vector<SQLSMALLINT> const & column_types)
    {
        auto statement = std::make_shared<mock_statement>();

        ON_CALL(*statement, do_number_of_columns())
            .WillByDefault(testing::Return(column_types.size()));

        for (std::size_t i = 0; i != column_types.size(); ++i) {
            cpp_odbc::column_description const value = {"dummy_name", column_types[i], 42, 17, true};
            ON_CALL(*statement, do_describe_column(i + 1))
                .WillByDefault(testing::Return(value));
            ON_CALL(*statement, do_describe_column_wide(i + 1)) // for both narrow and wide columns
                .WillByDefault(testing::Return(value));
        }
        return statement;
    }

}


TEST(DoubleBufferedResultSetTest, IsResultSet)
{
    bool const is_result_set = std::is_base_of<turbodbc::result_sets::result_set, double_buffered_result_set>::value;
    EXPECT_TRUE(is_result_set);
}


TEST(DoubleBufferedResultSetTest, BindsArraySizeFixedRowsInContructor)
{
    std::vector<SQLSMALLINT> const sql_column_types = {SQL_INTEGER, SQL_VARCHAR};
    std::vector<SQLSMALLINT> const c_column_types = {SQL_C_SBIGINT, SQL_CHAR};
    std::size_t const buffered_rows = 1001;

    auto statement = prepare_mock_with_columns(sql_column_types);
    EXPECT_CALL(*statement, do_set_attribute(SQL_ATTR_ROW_ARRAY_SIZE, 501))
        .Times(testing::AtLeast(1));

    double_buffered_result_set rs(statement, make_options(turbodbc::rows(buffered_rows), prefer_string));
}


TEST(DoubleBufferedResultSetTest, BindsArraySizeFixedMegabytesInContructor)
{
    std::vector<SQLSMALLINT> const sql_column_types = {SQL_INTEGER};
    std::vector<SQLSMALLINT> const c_column_types = {SQL_C_SBIGINT};
    // when the full buffer is set to 3 megabytes, use 2 megabytes per half buffer
    std::size_t const expected_rows_for_half_buffer = 2 * 1024 * 1024 / 8;

    auto statement = prepare_mock_with_columns(sql_column_types);
    EXPECT_CALL(*statement, do_set_attribute(SQL_ATTR_ROW_ARRAY_SIZE, expected_rows_for_half_buffer))
        .Times(testing::AtLeast(1));

    double_buffered_result_set rs(statement, make_options(turbodbc::megabytes(3), prefer_string));
}



TEST(DoubleBufferedResultSetTest, GetColumnInfo)
{
    auto statement = prepare_mock_with_columns({SQL_INTEGER, SQL_VARCHAR});

    double_buffered_result_set rs(statement, make_options(turbodbc::rows(123), prefer_string));

    ASSERT_EQ(2, rs.get_column_info().size());
    EXPECT_EQ(turbodbc::type_code::integer, rs.get_column_info()[0].type);
    EXPECT_EQ(turbodbc::type_code::string, rs.get_column_info()[1].type);
}


namespace {

    /**
     * This class is a fake statement which implements functions relevant for
     * the result set handling on top of a mock object.
     */
    class statement_with_fake_int_result_set : public mock_statement {
    public:
        statement_with_fake_int_result_set(std::vector<size_t> batch_sizes) :
            rows_fetched_pointer_(nullptr),
            buffer_(nullptr),
            batch_sizes_(std::move(batch_sizes)),
            batch_index_(0)
        {}

        short int do_number_of_columns() const final
        {
            return 1;
        }

        cpp_odbc::column_description do_describe_column(SQLUSMALLINT) const final
        {
            return {"dummy_name", SQL_INTEGER, 42, 17, true};
        }

        void do_set_attribute(SQLINTEGER attribute, SQLULEN * pointer) const final
        {
            if (attribute == SQL_ATTR_ROWS_FETCHED_PTR) {
                rows_fetched_pointer_ = pointer;
            }
        };

        void do_bind_column(SQLUSMALLINT, SQLSMALLINT, cpp_odbc::multi_value_buffer & buffer) const final
        {
            buffer_ = &buffer;
        }

        bool do_fetch_next() const final
        {
            if (batch_index_ < batch_sizes_.size()) {
                *rows_fetched_pointer_ = batch_sizes_[batch_index_];
                for (std::size_t i = 0; i != batch_sizes_[batch_index_]; ++i) {
                    auto element = (*buffer_)[i];
                    *reinterpret_cast<int64_t *>(element.data_pointer) = (batch_index_ + 1);
                    element.indicator = sizeof(intptr_t);
                }
                ++batch_index_;
            } else {
                *rows_fetched_pointer_ = 0;
            }
            return (*rows_fetched_pointer_ != 0);
        };

    private:
        mutable SQLULEN * rows_fetched_pointer_;
        mutable cpp_odbc::multi_value_buffer * buffer_;
        std::vector<size_t> batch_sizes_;
        mutable std::size_t batch_index_;
    };

}


TEST(DoubleBufferedResultSetTest, FetchNextBatch)
{
    std::vector<size_t> batch_sizes = {123};
    auto statement = std::make_shared<testing::NiceMock<statement_with_fake_int_result_set>>(batch_sizes);

    double_buffered_result_set rs(statement, make_options(turbodbc::rows(1000), prefer_string));
    EXPECT_EQ(123, rs.fetch_next_batch());
}


TEST(DoubleBufferedResultSetTest, GetBuffers)
{
    std::vector<size_t> batch_sizes = {4, 4, 2, 0};
    auto statement = std::make_shared<testing::NiceMock<statement_with_fake_int_result_set>>(batch_sizes);

    double_buffered_result_set rs(statement, make_options(turbodbc::rows(8), prefer_string));

    // first batch
    ASSERT_EQ(4, rs.fetch_next_batch());
    ASSERT_EQ(1, rs.get_buffers().size());
    EXPECT_EQ(1, *reinterpret_cast<int64_t const *>(rs.get_buffers()[0].get()[3].data_pointer));

    // second batch
    ASSERT_EQ(4, rs.fetch_next_batch());
    ASSERT_EQ(1, rs.get_buffers().size());
    EXPECT_EQ(2, *reinterpret_cast<intptr_t const *>(rs.get_buffers()[0].get()[3].data_pointer));

    // third batch (partially filled)
    ASSERT_EQ(2, rs.fetch_next_batch());
    ASSERT_EQ(1, rs.get_buffers().size());
    EXPECT_EQ(3, *reinterpret_cast<intptr_t const *>(rs.get_buffers()[0].get()[1].data_pointer));

    // fourth, non-existing batch
    ASSERT_EQ(0, rs.fetch_next_batch());
}


namespace {
    /**
     * This class is a fake statement which implements functions relevant for
     * the result set handling on top of a mock object.
     * THIS STATEMENT WILL THROW AN EXCEPTION WHILE FETCHING!
     */
    class statement_with_fake_failing_result_set : public mock_statement {
    public:
        statement_with_fake_failing_result_set() = default;

        short int do_number_of_columns() const final
        {
            return 1;
        }

        cpp_odbc::column_description do_describe_column(SQLUSMALLINT) const final
        {
            return {"dummy_name", SQL_INTEGER, 42, 17, true};
        }

        bool do_fetch_next() const final
        {
            throw std::runtime_error("FAILURE WHILE FETCHING");
        };
    };
}


TEST(DoubleBufferedResultSetTest, FetchNextBatchFails)
{
    auto statement = std::make_shared<testing::NiceMock<statement_with_fake_failing_result_set>>();

    double_buffered_result_set rs(statement, make_options(turbodbc::rows(1000), prefer_string));
    EXPECT_THROW(rs.fetch_next_batch(), std::runtime_error);
}


namespace {

    void test_preference(bool prefer_unicode, SQLSMALLINT expected_c_column_type)
    {
        std::vector<SQLSMALLINT> const sql_column_types = {SQL_VARCHAR}; // get character

        auto statement = prepare_mock_with_columns(sql_column_types);
        EXPECT_CALL(*statement, do_bind_column(1, expected_c_column_type, testing::_)).Times(testing::AtLeast(2));

        double_buffered_result_set rs(statement, make_options(turbodbc::rows(1), prefer_unicode));
    }

}

TEST(DoubleBufferedResultSetTest, ConstructorRespectsStringOrUnicodePreference)
{
    test_preference(prefer_string, SQL_CHAR);
    test_preference(prefer_unicode, SQL_WCHAR);
}
