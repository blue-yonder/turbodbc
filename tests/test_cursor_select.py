import datetime

import pytest
from helpers import (
    for_each_database,
    for_each_database_except,
    for_one_database,
    open_cursor,
)
from query_fixture import query_fixture

import turbodbc


def _test_single_row_result_set(configuration, query, expected_row):
    with open_cursor(configuration) as cursor:
        cursor.execute(query)

        if configuration["capabilities"]["supports_row_count"]:
            assert cursor.rowcount == 1
        else:
            assert cursor.rowcount == -1

        row = cursor.fetchone()
        assert row == expected_row

        row = cursor.fetchone()
        assert row is None


@for_each_database
def test_select_with_too_many_parameters_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with pytest.raises(turbodbc.Error):
            cursor.execute("SELECT 42", [42])

        with pytest.raises(turbodbc.Error):
            cursor.executemany("SELECT 42", [[42]])


@for_each_database
def test_select_single_row_NULL_result(dsn, configuration):
    _test_single_row_result_set(configuration, "SELECT NULL", [None])


@for_each_database
def test_select_single_row_integer_result(dsn, configuration):
    _test_single_row_result_set(configuration, "SELECT 42", [42])


@for_each_database
def test_select_single_row_bool_result(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT TRUE") as query:
            cursor.execute(query)
            row = cursor.fetchone()
            assert row == [True]
        with query_fixture(cursor, configuration, "SELECT FALSE") as query:
            cursor.execute(query)
            row = cursor.fetchone()
            assert row == [False]


@for_each_database
def test_select_single_row_string_result(dsn, configuration):
    _test_single_row_result_set(configuration, "SELECT 'value'", ["value"])


@for_each_database
def test_select_single_row_unicode_result(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT UNICODE") as query:
            cursor.execute(query)
            row = cursor.fetchone()
            assert row == ["I \u2665 unicode"]


@for_each_database
def test_select_single_row_double_result(configuration, dsn):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT DOUBLE") as query:
            cursor.execute(query)
            row = cursor.fetchone()
            assert row == [3.14]


@for_each_database
def test_select_single_row_date_result(configuration, dsn):
    _test_single_row_result_set(
        configuration,
        "SELECT CAST('2015-12-31' AS DATE) AS a",
        [datetime.date(2015, 12, 31)],
    )


@for_each_database
def test_select_single_row_timestamp_result(configuration, dsn):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT TIMESTAMP") as query:
            cursor.execute(query)
            row = cursor.fetchone()
            assert row == [datetime.datetime(2015, 12, 31, 1, 2, 3)]


@for_each_database
def test_select_single_row_large_numeric_result_as_string(configuration, dsn):
    _test_single_row_result_set(
        configuration, "SELECT -1234567890123.123456789", ["-1234567890123.123456789"]
    )


@for_each_database
def test_select_single_row_large_integer_decimal_as_64_bit_type(configuration, dsn):
    with open_cursor(configuration, large_decimals_as_64_bit_types=True) as cursor:
        with query_fixture(
            cursor, configuration, "SELECT LARGE INTEGER DECIMAL"
        ) as query:
            cursor.execute(query)
            row = cursor.fetchone()
            assert row == [42]


@for_each_database
def test_select_single_row_large_fractional_decimal_as_64_bit_type(configuration, dsn):
    with open_cursor(configuration, large_decimals_as_64_bit_types=True) as cursor:
        with query_fixture(
            cursor, configuration, "SELECT LARGE FRACTIONAL DECIMAL"
        ) as query:
            cursor.execute(query)
            row = cursor.fetchone()
            assert row == pytest.approx([3.14])


@for_each_database
def test_select_single_row_multiple_columns(configuration, dsn):
    _test_single_row_result_set(
        configuration, "SELECT 40, 41, 42, 43", [40, 41, 42, 43]
    )


@for_each_database
def test_fetchone(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT MULTIPLE INTEGERS") as query:
            cursor.execute(query)
            row = cursor.fetchone()
            assert row == [42]
            row = cursor.fetchone()
            assert row == [43]
            row = cursor.fetchone()
            assert row == [44]

            row = cursor.fetchone()
            assert row is None


@for_each_database
def test_fetchall(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT MULTIPLE INTEGERS") as query:
            cursor.execute(query)
            rows = cursor.fetchall()
            assert len(rows) == 3
            assert rows[0] == [42]
            assert rows[1] == [43]
            assert rows[2] == [44]


@for_each_database
def test_fetchmany_with_default_arraysize(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT MULTIPLE INTEGERS") as query:
            cursor.execute(query)
            rows = cursor.fetchmany()
            assert len(rows) == 1
            assert rows[0] == [42]


@for_each_database
def test_fetchmany_with_arraysize_parameter(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT MULTIPLE INTEGERS") as query:
            cursor.execute(query)
            arraysize_parameter = 2

            rows = cursor.fetchmany(arraysize_parameter)
            assert len(rows) == arraysize_parameter
            assert rows[0] == [42]
            assert rows[1] == [43]

            # arraysize exceeds number of remaining rows
            rows = cursor.fetchmany(arraysize_parameter)
            assert len(rows) == 1
            assert rows[0] == [44]


@for_each_database
def test_fetchmany_with_global_arraysize(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT MULTIPLE INTEGERS") as query:
            cursor.execute(query)

            arraysize_parameter = 2
            cursor.arraysize = arraysize_parameter

            rows = cursor.fetchmany()
            assert len(rows) == arraysize_parameter
            assert rows[0] == [42]
            assert rows[1] == [43]

            # arraysize exceeds number of remaining rows
            rows = cursor.fetchmany()
            assert len(rows) == 1
            assert rows[0] == [44]


@for_each_database
def test_fetchmany_with_bad_arraysize_parameter_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT MULTIPLE INTEGERS") as query:
            cursor.execute(query)

            with pytest.raises(turbodbc.InterfaceError):
                cursor.fetchmany(-1)
            with pytest.raises(turbodbc.InterfaceError):
                cursor.fetchmany(0)


@for_each_database
def test_fetchmany_with_bad_global_arraysize_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT MULTIPLE INTEGERS") as query:
            cursor.execute(query)

            cursor.arraysize = -1
            with pytest.raises(turbodbc.InterfaceError):
                cursor.fetchmany()

            cursor.arraysize = 0
            with pytest.raises(turbodbc.InterfaceError):
                cursor.fetchmany()


@for_each_database
def test_number_of_rows_exceeds_buffer_size(dsn, configuration):
    buffer_size = 3
    with open_cursor(configuration, rows_to_buffer=buffer_size) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            numbers = buffer_size * 2 + 1
            for i in range(numbers):
                cursor.execute(f"INSERT INTO {table_name} VALUES ({i})")

            cursor.execute(f"SELECT a FROM {table_name}")
            retrieved = cursor.fetchall()
            actual_sum = sum(row[0] for row in retrieved)
            expected_sum = sum(range(numbers))
            assert expected_sum == actual_sum


@for_each_database
def test_description(dsn, configuration):
    capabilities = configuration["capabilities"]

    with open_cursor(configuration) as cursor:
        assert cursor.description is None

        def fix_case(string):
            if capabilities["reports_column_names_as_upper_case"]:
                return string.upper()
            else:
                return string

        with query_fixture(cursor, configuration, "DESCRIPTION") as table_name:
            cursor.execute(f"SELECT * FROM {table_name}")

            nullness_for_null_column = not capabilities["indicates_null_columns"]

            expected = [
                (fix_case("as_int"), turbodbc.NUMBER, None, None, None, None, True),
                (fix_case("as_double"), turbodbc.NUMBER, None, None, None, None, True),
                (fix_case("as_varchar"), turbodbc.STRING, None, None, None, None, True),
                (fix_case("as_date"), turbodbc.DATETIME, None, None, None, None, True),
                (
                    fix_case("as_timestamp"),
                    turbodbc.DATETIME,
                    None,
                    None,
                    None,
                    None,
                    True,
                ),
                (
                    fix_case("as_int_not_null"),
                    turbodbc.NUMBER,
                    None,
                    None,
                    None,
                    None,
                    nullness_for_null_column,
                ),
            ]
            assert expected == cursor.description


@for_each_database_except(["MySQL"])
def test_unicode_column_names(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "UNICODE COLUMN NAME") as table_name:
            cursor.execute(f"SELECT * FROM {table_name}")
            assert cursor.description[0][0] == "I \u2665 Unicode"


@for_one_database
def test_execute_supports_chaining(dsn, configuration):
    with open_cursor(configuration) as cursor:
        rows = cursor.execute("SELECT 42").fetchall()
        assert rows == [[42]]


@for_one_database
def test_executemany_supports_chaining(dsn, configuration):
    with open_cursor(configuration) as cursor:
        rows = cursor.executemany("SELECT 42").fetchall()
        assert rows == [[42]]
