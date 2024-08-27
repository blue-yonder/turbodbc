import datetime
from typing import Any, cast

from helpers import (
    for_each_database,
    for_one_database,
    generate_microseconds_with_precision,
    open_cursor,
)
from query_fixture import query_fixture


def _test_insert_many(configuration, fixture_name, data):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, fixture_name) as table_name:
            cursor.executemany(f"INSERT INTO {table_name} VALUES (?)", data)
            assert len(data) == cursor.rowcount
            cursor.execute(f"SELECT a FROM {table_name} ORDER BY a")
            inserted = cursor.fetchall()
            data = [list(row) for row in data]
            assert data == inserted


def _test_insert_one(configuration, fixture_name, data):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, fixture_name) as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", data)
            assert 1 == cursor.rowcount
            cursor.execute(f"SELECT a FROM {table_name}")
            inserted = cursor.fetchall()
            assert [list(data)] == inserted


@for_one_database
def test_execute_with_tuple(dsn, configuration):
    as_tuple = (1,)
    _test_insert_one(configuration, "INSERT INTEGER", as_tuple)


@for_each_database
def test_insert_with_execute(dsn, configuration):
    as_list = [1]
    _test_insert_one(configuration, "INSERT INTEGER", as_list)


@for_each_database
def test_insert_string_column(dsn, configuration):
    _test_insert_many(
        configuration, "INSERT STRING", [["hello"], ["my"], ["test case"]]
    )


@for_each_database
def test_insert_string_max_column(dsn, configuration):
    _test_insert_many(
        configuration, "INSERT STRING MAX", [["hello"], ["my"], ["test case"]]
    )


@for_each_database
def test_insert_string_column_with_truncation(dsn, configuration):
    with open_cursor(
        configuration, varchar_max_character_limit=9, limit_varchar_results_to_max=True
    ) as cursor:
        with query_fixture(cursor, configuration, "INSERT LONG STRING") as table_name:
            cursor.execute(
                f"INSERT INTO {table_name} VALUES (?)",
                ["Truncated strings suck"],
            )
            cursor.execute(f"SELECT a FROM {table_name}")

            assert cursor.fetchall() == [["Truncated"]]


@for_each_database
def test_insert_unicode_column(dsn, configuration):
    _test_insert_many(
        configuration,
        "INSERT UNICODE",
        [["a I \u2665 unicode"], ["b I really d\u00f8"]],
    )


@for_each_database
def test_insert_unicode_max_column(dsn, configuration):
    _test_insert_many(
        configuration,
        "INSERT UNICODE MAX",
        [["a I \u2665 unicode"], ["b I really d\u00f8"]],
    )


@for_each_database
def test_insert_unicode_column_with_truncation(dsn, configuration):
    with open_cursor(
        configuration, varchar_max_character_limit=4, limit_varchar_results_to_max=True
    ) as cursor:
        with query_fixture(cursor, configuration, "INSERT UNICODE MAX") as table_name:
            cursor.execute(
                f"INSERT INTO {table_name} VALUES (?)", ["I \u2665 truncated"]
            )
            cursor.execute(f"SELECT a FROM {table_name}")

            # depending on the database and the settings, this test may cut through the
            # multi-byte UTF-8 representation of \u2665, or it may not if UTF-16 characters
            # are used. Hence, the assertions are more fuzzy than expected.
            truncated = cursor.fetchall()[0]
            assert len(truncated) > 0
            assert len(truncated) <= 4


@for_each_database
def test_insert_bool_column(dsn, configuration):
    _test_insert_many(configuration, "INSERT BOOL", [[False], [True], [True]])


@for_one_database
def test_execute_many_with_tuple(dsn, configuration):
    _test_insert_many(configuration, "INSERT INTEGER", [(1,), (2,), (3,)])


@for_each_database
def test_insert_integer_column(dsn, configuration):
    _test_insert_many(configuration, "INSERT INTEGER", [[1], [2], [3]])


@for_each_database
def test_insert_double_column(dsn, configuration):
    _test_insert_many(configuration, "INSERT DOUBLE", [[1.23], [2.71], [3.14]])


@for_each_database
def test_insert_date_column(dsn, configuration):
    _test_insert_many(
        configuration,
        "INSERT DATE",
        [
            [datetime.date(2015, 12, 31)],
            [datetime.date(2016, 1, 15)],
            [datetime.date(2016, 2, 3)],
        ],
    )


@for_each_database
def test_insert_timestamp_column(dsn, configuration):
    supported_digits = configuration["capabilities"]["fractional_second_digits"]
    fractional = generate_microseconds_with_precision(supported_digits)

    _test_insert_many(
        configuration,
        "INSERT TIMESTAMP",
        [
            [datetime.datetime(2015, 12, 31, 1, 2, 3, fractional)],
            [datetime.datetime(2016, 1, 15, 4, 5, 6, fractional * 2)],
            [datetime.datetime(2016, 2, 3, 7, 8, 9, fractional * 3)],
        ],
    )


@for_each_database
def test_insert_null(dsn, configuration):
    _test_insert_many(configuration, "INSERT INTEGER", [[None]])


@for_each_database
def test_insert_mixed_data_columns(dsn, configuration):
    # second column has mixed data types in the same column
    # first column makes sure values of "good" columns are not affected
    to_insert = [[23, 1.23], [42, 2]]

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT MIXED") as table_name:
            cursor.executemany(f"INSERT INTO {table_name} VALUES (?, ?)", to_insert)
            assert len(to_insert) == cursor.rowcount
            cursor.execute(f"SELECT a, b FROM {table_name} ORDER BY a")
            inserted = [list(row) for row in cursor.fetchall()]
            assert to_insert == inserted


@for_each_database
def test_insert_no_parameter_list(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.executemany(f"INSERT INTO {table_name} VALUES (?)")
            assert 0 == cursor.rowcount
            cursor.execute(f"SELECT a FROM {table_name}")
            inserted = [list(row) for row in cursor.fetchall()]
            assert 0 == len(inserted)


@for_each_database
def test_insert_empty_parameter_list(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.executemany(f"INSERT INTO {table_name} VALUES (?)", [])
            assert 0 == cursor.rowcount
            cursor.execute(f"SELECT a FROM {table_name}")
            inserted = [list(row) for row in cursor.fetchall()]
            assert [] == inserted


@for_each_database
def test_insert_number_of_rows_exceeds_buffer_size(dsn, configuration):
    buffer_size = 3
    numbers = buffer_size * 2 + 1
    data = [[i] for i in range(numbers)]

    with open_cursor(configuration, parameter_sets_to_buffer=buffer_size) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.executemany(f"INSERT INTO {table_name} VALUES (?)", data)
            assert len(data) == cursor.rowcount
            cursor.execute(f"SELECT a FROM {table_name} ORDER BY a")
            inserted = [list(row) for row in cursor.fetchall()]
            assert data == inserted


@for_each_database
def test_description_after_insert(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (42)")
            assert cursor.description is None


@for_each_database
def test_string_with_differing_lengths(dsn, configuration):
    long_strings = [["x" * 5], ["x" * 50], ["x" * 500]]
    # use integer to force rebind to string buffer afterwards
    to_insert = cast(list[list[Any]], [[1]]) + long_strings
    expected = [["1"]] + long_strings

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT LONG STRING") as table_name:
            cursor.executemany(f"INSERT INTO {table_name} VALUES (?)", to_insert)
            assert len(to_insert) == cursor.rowcount
            cursor.execute(f"SELECT a FROM {table_name}")
            inserted = [list(row) for row in cursor.fetchall()]
            assert expected == inserted


@for_each_database
def test_rowcount_works_without_parameter_sets(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (42), (17)")
            assert cursor.rowcount == 2
