#include <turbodbc_arrow/arrow_result_set.h>

#include <list>
#include <random>

#undef BOOL
#undef timezone
#include <arrow/api.h>
#include <arrow/testing/gtest_util.h>
#include <arrow/testing/util.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sql.h>

using arrow::StringDictionaryBuilder;

namespace {

    const int64_t OUTPUT_SIZE = 100;
    const std::size_t size_unimportant = 8;

    bool const strings_as_strings = false;
    bool const strings_as_dictionaries = true;

    bool const plain_integers = false;
    bool const compressed_integers = true;

    struct mock_result_set : public turbodbc::result_sets::result_set {
        MOCK_METHOD0(do_fetch_next_batch, std::size_t());
        MOCK_CONST_METHOD0(do_get_column_info, std::vector<turbodbc::column_info>());
        MOCK_CONST_METHOD0(do_get_buffers, std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>>());
    };

    template <typename ArrowType>
    void make_int_range(int64_t size, std::shared_ptr<arrow::Array>* out) {
        typename arrow::TypeTraits<ArrowType>::BuilderType builder;
        for (int64_t i = 0; i < size; i++) {
          ASSERT_OK(builder.Append(i));
        }
        std::shared_ptr<arrow::Array> array;
        ASSERT_OK(builder.Finish(out));
    }

    void random_bytes(int64_t n, uint32_t seed, uint8_t* out) {
        std::default_random_engine gen(seed);
        std::uniform_int_distribution<uint32_t> d(0, std::numeric_limits<uint8_t>::max());
        std::generate(out, out + n, [&d, &gen] { return static_cast<uint8_t>(d(gen)); });
    }

}

class ArrowResultSetTest : public ::testing::Test {
    public:
        void SetUp() {
            pool = arrow::default_memory_pool();
        }

        void TearDown() {
            buffers.clear();
            expected_arrays.clear();
            expected_fields.clear();
        }

        void MockSchema(std::vector<turbodbc::column_info> schema) {
            EXPECT_CALL(rs, do_get_column_info()).WillRepeatedly(testing::Return(schema));
        }

        // This only works on PrimitiveArrays of a FixedWidthType
        std::reference_wrapper<cpp_odbc::multi_value_buffer const>
        BufferFromPrimitive(std::shared_ptr<arrow::Array> const& array, size_t size, size_t offset) {
            size_t bytes_per_value = std::dynamic_pointer_cast<arrow::FixedWidthType>(array->type())->bit_width() / 8;
            auto typed_array = std::dynamic_pointer_cast<arrow::PrimitiveArray>(array);
            buffers.emplace_back(cpp_odbc::multi_value_buffer(bytes_per_value, size));
            memcpy(buffers.back().data_pointer(), typed_array->values()->data() + (bytes_per_value * offset), bytes_per_value * size);
            for (size_t i = 0; i < size; i++) {
                if (array->IsNull(i + offset)) {
                    buffers.back().indicator_pointer()[i] = SQL_NULL_DATA;
                }
            }
            return buffers.back();
        }

        void MockOutput(std::vector<std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>>> buffers_vec) {
            {
                testing::InSequence sequence;
                for (auto&& buffers: buffers_vec) {
                    EXPECT_CALL(rs, do_get_buffers()).WillOnce(testing::Return(buffers)).RetiresOnSaturation();
                }
            }
            {
                testing::InSequence sequence;
                for (auto&& buffers: buffers_vec) {
                    EXPECT_CALL(rs, do_fetch_next_batch()).WillOnce(testing::Return(buffers[0].get().number_of_elements())).RetiresOnSaturation();
                }
                EXPECT_CALL(rs, do_fetch_next_batch()).WillOnce(testing::Return(0)).RetiresOnSaturation();
            }
        }

        template <typename ArrayType>
        std::shared_ptr<arrow::Array> MakePrimitive(int64_t length, int64_t null_count = 0) {
            std::shared_ptr<arrow::ResizableBuffer> data;
            const int64_t data_nbytes = length * sizeof(typename ArrayType::value_type);
            data = *AllocateResizableBuffer(data_nbytes, pool);

            // Fill with random data
            random_bytes(data_nbytes, 0 /*random_seed*/, data->mutable_data());

            std::shared_ptr<arrow::ResizableBuffer> null_bitmap;
#if ARROW_VERSION_MAJOR >= 7
            const int64_t null_nbytes = arrow::bit_util::BytesForBits(length);
#else
            const int64_t null_nbytes = arrow::BitUtil::BytesForBits(length);
#endif
            null_bitmap = *AllocateResizableBuffer(null_nbytes, pool);
            memset(null_bitmap->mutable_data(), 255, null_nbytes);
            for (int64_t i = 0; i < null_count; i++) {
#if ARROW_VERSION_MAJOR >= 7
                arrow::bit_util::ClearBit(null_bitmap->mutable_data(), i * (length / null_count));
#else
                arrow::BitUtil::ClearBit(null_bitmap->mutable_data(), i * (length / null_count));
#endif
            }
            return std::make_shared<ArrayType>(length, data, null_bitmap, null_count);
        }

        void CheckRoundtrip(bool strings_as_dictionary, bool adaptive_integers) {
            auto schema = std::make_shared<arrow::Schema>(expected_fields);
            std::shared_ptr<arrow::Table> expected_table = arrow::Table::Make(schema, expected_arrays);

            turbodbc_arrow::arrow_result_set ars(rs, strings_as_dictionary, adaptive_integers);
            std::shared_ptr<arrow::Table> table;
            ASSERT_OK(ars.fetch_all_native(&table, false));
            ASSERT_TRUE(expected_table->Equals(*table));
        }

    protected:
        mock_result_set rs;
        arrow::MemoryPool* pool;
        // Keep buffers in a list, so you can keep references to the instances.
        std::list<cpp_odbc::multi_value_buffer> buffers;

        std::vector<std::shared_ptr<arrow::Array>> expected_arrays;
        std::vector<std::shared_ptr<arrow::Field>> expected_fields;
};

TEST_F(ArrowResultSetTest, SimpleSchemaConversion)
{
    std::vector<turbodbc::column_info> expected = {{
        "int_column", turbodbc::type_code::integer, size_unimportant, true}};
    EXPECT_CALL(rs, do_get_column_info()).WillRepeatedly(testing::Return(expected));

    turbodbc_arrow::arrow_result_set ars(rs, strings_as_strings, plain_integers);
    auto schema = ars.schema();
    ASSERT_EQ(schema->num_fields(), 1);
    auto field = schema->field(0);
    ASSERT_EQ(field->name(), "int_column");
    ASSERT_EQ(field->type(), arrow::int64());
    ASSERT_EQ(field->nullable(), true);
}

TEST_F(ArrowResultSetTest, AllTypesSchemaConversion)
{
    MockSchema({
        {"float_column", turbodbc::type_code::floating_point, size_unimportant, true},
        {"boolean_column", turbodbc::type_code::boolean, size_unimportant, true},
        {"timestamp_column", turbodbc::type_code::timestamp, size_unimportant, true},
        {"date_column", turbodbc::type_code::date, size_unimportant, true},
        {"string_column", turbodbc::type_code::string, size_unimportant, true},
        {"int_column", turbodbc::type_code::integer, size_unimportant, true},
        {"nonnull_float_column", turbodbc::type_code::floating_point, size_unimportant, false},
        {"nonnull_boolean_column", turbodbc::type_code::boolean, size_unimportant, false},
        {"nonnull_timestamp_column", turbodbc::type_code::timestamp, size_unimportant, false},
        {"nonnull_date_column", turbodbc::type_code::date, size_unimportant, false},
        {"nonnull_string_column", turbodbc::type_code::string, size_unimportant, false},
        {"nonnull_int_column", turbodbc::type_code::integer, size_unimportant, false}});

    std::vector<std::shared_ptr<arrow::Field>> expected_fields = {
        std::make_shared<arrow::Field>("float_column", arrow::float64()),
        std::make_shared<arrow::Field>("boolean_column", arrow::boolean()),
        std::make_shared<arrow::Field>("timestamp_column", arrow::timestamp(arrow::TimeUnit::MICRO)),
        std::make_shared<arrow::Field>("date_column", arrow::date32()),
        std::make_shared<arrow::Field>("string_column", std::make_shared<arrow::StringType>()),
        std::make_shared<arrow::Field>("int_column", arrow::int64()),
        std::make_shared<arrow::Field>("nonnull_float_column", arrow::float64(), false),
        std::make_shared<arrow::Field>("nonnull_boolean_column", arrow::boolean(), false),
        std::make_shared<arrow::Field>("nonnull_timestamp_column", arrow::timestamp(arrow::TimeUnit::MICRO), false),
        std::make_shared<arrow::Field>("nonnull_date_column", arrow::date32(), false),
        std::make_shared<arrow::Field>("nonnull_string_column", std::make_shared<arrow::StringType>(), false),
        std::make_shared<arrow::Field>("nonnull_int_column", arrow::int64(), false)
    };

    turbodbc_arrow::arrow_result_set ars(rs, strings_as_strings, plain_integers);
    auto schema = ars.schema();

    ASSERT_EQ(schema->num_fields(), 12);
    for (int i = 0; i < schema->num_fields(); i++) {
        EXPECT_TRUE(schema->field(i)->Equals(expected_fields[i]));
    }
}

TEST_F(ArrowResultSetTest, SingleBatchSingleColumnResultSetConversion)
{
    // Expected output: a Table with a single column of int64s
    arrow::Int64Builder builder;
    for (int64_t i = 0; i < OUTPUT_SIZE; i++) {
        ASSERT_OK(builder.Append(i));
    }
    std::shared_ptr<arrow::Array> array;
    ASSERT_OK(builder.Finish(&array));
    std::shared_ptr<arrow::Int64Array> typed_array = std::static_pointer_cast<arrow::Int64Array>(array);
    std::vector<std::shared_ptr<arrow::Field>> fields({std::make_shared<arrow::Field>("int_column", arrow::int64(), true)});
    std::shared_ptr<arrow::Schema> schema = std::make_shared<arrow::Schema>(fields);

    std::shared_ptr<arrow::Table> expected_table = arrow::Table::Make(schema, {array});

    MockSchema({{"int_column", turbodbc::type_code::integer, size_unimportant, true}});

    // Mock output columns
    // * Single batch of 100 ints
    cpp_odbc::multi_value_buffer buffer(sizeof(int64_t), OUTPUT_SIZE);
    memcpy(buffer.data_pointer(), typed_array->values()->data(), sizeof(int64_t) * OUTPUT_SIZE);
    std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> expected_buffers = {buffer};
    EXPECT_CALL(rs, do_get_buffers()).WillOnce(testing::Return(expected_buffers));
    EXPECT_CALL(rs, do_fetch_next_batch()).WillOnce(testing::Return(OUTPUT_SIZE)).WillOnce(testing::Return(0));

    turbodbc_arrow::arrow_result_set ars(rs, strings_as_strings, plain_integers);
    std::shared_ptr<arrow::Table> table;
    ASSERT_OK(ars.fetch_all_native(&table, false));
    ASSERT_TRUE(expected_table->Equals(*table));
}

TEST_F(ArrowResultSetTest, MultiBatchConversionInteger)
{
    std::shared_ptr<arrow::Array> array = MakePrimitive<arrow::Int64Array>(2 * OUTPUT_SIZE, OUTPUT_SIZE / 3);
    expected_arrays.push_back(array);
    expected_fields.push_back(arrow::field("int_column", arrow::int64(), true));
    std::shared_ptr<arrow::Array> nonnull_array = MakePrimitive<arrow::Int64Array>(2 * OUTPUT_SIZE);
    expected_arrays.push_back(nonnull_array);
    expected_fields.push_back(arrow::field("nonnull_int_column", arrow::int64(), false));

    MockSchema({{"int_column", turbodbc::type_code::integer, size_unimportant, true},
            {"nonnull_int_column", turbodbc::type_code::integer, size_unimportant, false}});
    MockOutput({{BufferFromPrimitive(array, OUTPUT_SIZE, 0), BufferFromPrimitive(nonnull_array, OUTPUT_SIZE, 0)},
            {BufferFromPrimitive(array, OUTPUT_SIZE, OUTPUT_SIZE), BufferFromPrimitive(nonnull_array, OUTPUT_SIZE, OUTPUT_SIZE)}});
    CheckRoundtrip(strings_as_strings, plain_integers);
}

TEST_F(ArrowResultSetTest, ConversionCompressedInteger)
{
    std::shared_ptr<arrow::Array> array;
    make_int_range<arrow::Int64Type>(1024, &array);

    MockSchema({{"int_column", turbodbc::type_code::integer, size_unimportant, true}});
    MockOutput({{BufferFromPrimitive(array, 512, 0)},
            {BufferFromPrimitive(array, 512, 512)}});

    std::shared_ptr<arrow::Array> expected_array;
    make_int_range<arrow::Int16Type>(1024, &expected_array);
    expected_arrays.push_back(expected_array);
    expected_fields.push_back(arrow::field("int_column", arrow::int16(), true));

    CheckRoundtrip(strings_as_strings, compressed_integers);
}

TEST_F(ArrowResultSetTest, MultiBatchConversionFloat)
{
    std::shared_ptr<arrow::Array> array = MakePrimitive<arrow::DoubleArray>(2 * OUTPUT_SIZE, OUTPUT_SIZE / 3);
    expected_arrays.push_back(array);
    expected_fields.push_back(arrow::field("float_column", arrow::float64(), true));
    std::shared_ptr<arrow::Array> nonnull_array = MakePrimitive<arrow::DoubleArray>(2 * OUTPUT_SIZE);
    expected_arrays.push_back(nonnull_array);
    expected_fields.push_back(arrow::field("nonnull_float_column", arrow::float64(), false));

    MockSchema({{"float_column", turbodbc::type_code::floating_point, size_unimportant, true},
            {"nonnull_float_column", turbodbc::type_code::floating_point, size_unimportant, false}});
    MockOutput({{BufferFromPrimitive(array, OUTPUT_SIZE, 0), BufferFromPrimitive(nonnull_array, OUTPUT_SIZE, 0)},
            {BufferFromPrimitive(array, OUTPUT_SIZE, OUTPUT_SIZE), BufferFromPrimitive(nonnull_array, OUTPUT_SIZE, OUTPUT_SIZE)}});
    CheckRoundtrip(strings_as_strings, plain_integers);
}

TEST_F(ArrowResultSetTest, MultiBatchConversionBoolean)
{
    std::shared_ptr<arrow::Array> array;
    cpp_odbc::multi_value_buffer buffer_1(sizeof(bool), OUTPUT_SIZE);
    cpp_odbc::multi_value_buffer buffer_1_2(sizeof(bool), OUTPUT_SIZE);
    {
        arrow::BooleanBuilder builder;
        for (int64_t i = 0; i < 2 * OUTPUT_SIZE; i++) {
            if (i % 5 == 0) {
                ASSERT_OK(builder.AppendNull());
                if (i < OUTPUT_SIZE) {
                    buffer_1.indicator_pointer()[i] = SQL_NULL_DATA;
                } else {
                    buffer_1_2.indicator_pointer()[i - OUTPUT_SIZE] = SQL_NULL_DATA;
                }
            } else {
                ASSERT_OK(builder.Append(i % 3 == 0));
                if (i < OUTPUT_SIZE) {
                    *(buffer_1[i].data_pointer) = (i % 3 == 0);
                } else {
                    *(buffer_1_2[i - OUTPUT_SIZE].data_pointer) = (i % 3 == 0);
                }
            }
        }
        ASSERT_OK(builder.Finish(&array));
    }
    expected_arrays.push_back(array);
    expected_fields.push_back(arrow::field("bool_column", arrow::boolean(), true));

    std::shared_ptr<arrow::Array> nonnull_array;
    cpp_odbc::multi_value_buffer buffer_2(sizeof(bool), OUTPUT_SIZE);
    cpp_odbc::multi_value_buffer buffer_2_2(sizeof(bool), OUTPUT_SIZE);
    {
        arrow::BooleanBuilder builder;
        for (int64_t i = 0; i < 2 * OUTPUT_SIZE; i++) {
            ASSERT_OK(builder.Append(i % 3 == 0));
            if (i < OUTPUT_SIZE) {
                *(buffer_2[i].data_pointer) = (i % 3 == 0);
            } else {
                *(buffer_2_2[i - OUTPUT_SIZE].data_pointer) = (i % 3 == 0);
            }
        }
        ASSERT_OK(builder.Finish(&nonnull_array));
    }
    expected_arrays.push_back(nonnull_array);
    expected_fields.push_back(arrow::field("nonnull_bool_column", arrow::boolean(), false));

    MockSchema({{"bool_column", turbodbc::type_code::boolean, size_unimportant, true},
            {"nonnull_bool_column", turbodbc::type_code::boolean, size_unimportant, false}});
    MockOutput({{buffer_1, buffer_2}, {buffer_1_2, buffer_2_2}});
    CheckRoundtrip(strings_as_strings, plain_integers);
}

TEST_F(ArrowResultSetTest, MultiBatchConversionString)
{
    std::shared_ptr<arrow::Array> array;
    // Longest string: "200" -> 4 bytes
    cpp_odbc::multi_value_buffer buffer_1(4, OUTPUT_SIZE);
    cpp_odbc::multi_value_buffer buffer_1_2(4, OUTPUT_SIZE);
    {
        arrow::StringBuilder builder(pool);
        for (int64_t i = 0; i < 2 * OUTPUT_SIZE; i++) {
            if (i % 5 == 0) {
                ASSERT_OK(builder.AppendNull());
                if (i < OUTPUT_SIZE) {
                    buffer_1.indicator_pointer()[i] = SQL_NULL_DATA;
                } else {
                    buffer_1_2.indicator_pointer()[i - OUTPUT_SIZE] = SQL_NULL_DATA;
                }
            } else {
                std::string str = std::to_string(i);
                ASSERT_OK(builder.Append(str));
                if (i < OUTPUT_SIZE) {
                    memcpy(buffer_1[i].data_pointer, str.c_str(), str.size() + 1);
                    buffer_1[i].indicator = str.size();
                } else {
                    memcpy(buffer_1_2[i - OUTPUT_SIZE].data_pointer, str.c_str(), str.size() + 1);
                    buffer_1_2[i - OUTPUT_SIZE].indicator = str.size();
                }
            }
        }
        ASSERT_OK(builder.Finish(&array));
    }
    expected_arrays.push_back(array);
    expected_fields.push_back(arrow::field("str_column", arrow::utf8(), true));

    std::shared_ptr<arrow::Array> nonnull_array;
    cpp_odbc::multi_value_buffer buffer_2(4, OUTPUT_SIZE);
    cpp_odbc::multi_value_buffer buffer_2_2(4, OUTPUT_SIZE);
    {
        arrow::StringBuilder builder(pool);
        for (int64_t i = 0; i < 2 * OUTPUT_SIZE; i++) {
            std::string str = std::to_string(i);
            ASSERT_OK(builder.Append(str));
            if (i < OUTPUT_SIZE) {
                memcpy(buffer_2[i].data_pointer, str.c_str(), str.size() + 1);
                buffer_2[i].indicator = str.size();
            } else {
                memcpy(buffer_2_2[i - OUTPUT_SIZE].data_pointer, str.c_str(), str.size() + 1);
                buffer_2_2[i - OUTPUT_SIZE].indicator = str.size();
            }
        }
        ASSERT_OK(builder.Finish(&nonnull_array));
    }
    expected_arrays.push_back(nonnull_array);
    expected_fields.push_back(arrow::field("nonnull_str_column", arrow::utf8(), false));

    MockSchema({{"str_column", turbodbc::type_code::string, size_unimportant, true},
            {"nonnull_str_column", turbodbc::type_code::string, size_unimportant, false}});
    MockOutput({{buffer_1, buffer_2}, {buffer_1_2, buffer_2_2}});
    CheckRoundtrip(strings_as_strings, plain_integers);

    // Convert to dictionaries
    for (size_t i = 0; i < expected_arrays.size(); i++) {
        StringDictionaryBuilder builder(arrow::default_memory_pool());
        ASSERT_OK(builder.AppendArray(*expected_arrays[i]));
        ASSERT_OK(builder.Finish(&expected_arrays[i]));
        expected_fields[i] = arrow::field(expected_fields[i]->name(),
           expected_arrays[i]->type(), expected_fields[i]->nullable());
    }

    MockSchema({{"str_column", turbodbc::type_code::string, size_unimportant, true},
            {"nonnull_str_column", turbodbc::type_code::string, size_unimportant, false}});
    MockOutput({{buffer_1, buffer_2}, {buffer_1_2, buffer_2_2}});
    CheckRoundtrip(strings_as_dictionaries, plain_integers);
}

TEST_F(ArrowResultSetTest, MultiBatchConversionTimestamp)
{
    std::shared_ptr<arrow::Array> array;
    cpp_odbc::multi_value_buffer buffer_1(sizeof(SQL_TIMESTAMP_STRUCT), OUTPUT_SIZE);
    cpp_odbc::multi_value_buffer buffer_1_2(sizeof(SQL_TIMESTAMP_STRUCT), OUTPUT_SIZE);
    {
        arrow::TimestampBuilder builder(arrow::timestamp(arrow::TimeUnit::MICRO), pool);
        for (int64_t i = 0; i < 2 * OUTPUT_SIZE; i++) {
            if (i % 5 == 0) {
                ASSERT_OK(builder.AppendNull());
                if (i < OUTPUT_SIZE) {
                    buffer_1.indicator_pointer()[i] = SQL_NULL_DATA;
                } else {
                    buffer_1_2.indicator_pointer()[i - OUTPUT_SIZE] = SQL_NULL_DATA;
                }
            } else {
                ASSERT_OK(builder.Append(i));
                auto td = boost::posix_time::microseconds(i);
                auto ts = boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), td);
                SQL_TIMESTAMP_STRUCT* sql_ts;
                if (i < OUTPUT_SIZE) {
                    sql_ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT*>(buffer_1.data_pointer()) + i;
                } else {
                    sql_ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT*>(buffer_1_2.data_pointer()) + (i - OUTPUT_SIZE);
                }
                sql_ts->year = ts.date().year();
                sql_ts->month = ts.date().month();
                sql_ts->day = ts.date().day();
                sql_ts->hour = ts.time_of_day().hours();
                sql_ts->minute = ts.time_of_day().minutes();
                sql_ts->second = ts.time_of_day().seconds();
                sql_ts->fraction = ts.time_of_day().fractional_seconds() * 1000;
            }
        }
        ASSERT_OK(builder.Finish(&array));
    }
    expected_arrays.push_back(array);
    expected_fields.push_back(arrow::field("timestamp_column", arrow::timestamp(arrow::TimeUnit::MICRO), true));

    std::shared_ptr<arrow::Array> nonnull_array;
    cpp_odbc::multi_value_buffer buffer_2(sizeof(SQL_TIMESTAMP_STRUCT), OUTPUT_SIZE);
    cpp_odbc::multi_value_buffer buffer_2_2(sizeof(SQL_TIMESTAMP_STRUCT), OUTPUT_SIZE);
    {
        arrow::TimestampBuilder builder(arrow::timestamp(arrow::TimeUnit::MICRO), pool);
        for (int64_t i = 0; i < 2 * OUTPUT_SIZE; i++) {
            ASSERT_OK(builder.Append(i));
            auto td = boost::posix_time::microseconds(i);
            auto ts = boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), td);
            SQL_TIMESTAMP_STRUCT* sql_ts;
            if (i < OUTPUT_SIZE) {
                sql_ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT*>(buffer_2.data_pointer()) + i;
            } else {
                sql_ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT*>(buffer_2_2.data_pointer()) + (i - OUTPUT_SIZE);
            }
            sql_ts->year = ts.date().year();
            sql_ts->month = ts.date().month();
            sql_ts->day = ts.date().day();
            sql_ts->hour = ts.time_of_day().hours();
            sql_ts->minute = ts.time_of_day().minutes();
            sql_ts->second = ts.time_of_day().seconds();
            sql_ts->fraction = ts.time_of_day().fractional_seconds() * 1000;
        }
        ASSERT_OK(builder.Finish(&nonnull_array));
    }
    expected_arrays.push_back(nonnull_array);
    expected_fields.push_back(arrow::field("nonnull_timestamp_column", arrow::timestamp(arrow::TimeUnit::MICRO), false));

    MockSchema({{"timestamp_column", turbodbc::type_code::timestamp, size_unimportant, true},
            {"nonnull_timestamp_column", turbodbc::type_code::timestamp, size_unimportant, false}});
    MockOutput({{buffer_1, buffer_2}, {buffer_1_2, buffer_2_2}});
    CheckRoundtrip(strings_as_strings, plain_integers);
}
