import datetime
from collections import OrderedDict

import numpy
import pytest
from helpers import (
    for_each_database,
    for_one_database,
    generate_microseconds_with_precision,
    open_cursor,
)
from numpy.ma import MaskedArray
from numpy.testing import assert_equal
from query_fixture import query_fixture

import turbodbc


def _fix_case(configuration, string):
    """
    some databases return column names in upper case
    """
    capabilities = configuration["capabilities"]
    if capabilities["reports_column_names_as_upper_case"]:
        return string.upper()
    else:
        return string


@for_one_database
def test_numpy_without_result_set_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with pytest.raises(turbodbc.InterfaceError):
            cursor.fetchallnumpy()


@for_each_database
def test_numpy_empty_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            assert isinstance(results, OrderedDict)
            assert len(results) == 1  # ncols
            assert isinstance(results[_fix_case(configuration, "a")], MaskedArray)


@for_each_database
def test_numpy_empty_column_batch_fetch(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.execute(f"SELECT a FROM {table_name}")
            batches = cursor.fetchnumpybatches()
            for idx, batch in enumerate(batches):
                assert isinstance(batch, OrderedDict)
                assert len(batch) == 1  # ncols
                assert isinstance(batch[_fix_case(configuration, "a")], MaskedArray)
                assert_equal(len(batch[_fix_case(configuration, "a")]), 0)
            assert_equal(idx, 0)


@for_each_database
def test_numpy_int_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        cursor.execute("SELECT 42 AS a")
        results = cursor.fetchallnumpy()
        expected = MaskedArray([42], mask=[0])
        assert results[_fix_case(configuration, "a")].dtype == numpy.int64
        assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_double_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "SELECT DOUBLE") as query:
            cursor.execute(query)
            results = cursor.fetchallnumpy()
            expected = MaskedArray([3.14], mask=[0])
            assert results[_fix_case(configuration, "a")].dtype == numpy.float64
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_boolean_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INDEXED BOOL") as table_name:
            cursor.executemany(
                f"INSERT INTO {table_name} VALUES (?, ?)",
                [[True, 1], [False, 2], [True, 3]],
            )
            cursor.execute(f"SELECT a FROM {table_name} ORDER BY b")
            results = cursor.fetchallnumpy()
            expected = MaskedArray([True, False, True], mask=[0])
            assert results[_fix_case(configuration, "a")].dtype == numpy.bool_
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_binary_column_with_null(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(
            cursor, configuration, "INSERT TWO INTEGER COLUMNS"
        ) as table_name:
            cursor.executemany(
                f"INSERT INTO {table_name} VALUES (?, ?)", [[42, 1], [None, 2]]
            )  # second column to enforce ordering
            cursor.execute(f"SELECT a FROM {table_name} ORDER BY b")
            results = cursor.fetchallnumpy()
            expected = MaskedArray([42, 0], mask=[0, 1])
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_binary_column_larger_than_batch_size(dsn, configuration):
    with open_cursor(configuration, rows_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.executemany(
                f"INSERT INTO {table_name} VALUES (?)",
                [[1], [2], [3], [4], [5]],
            )
            cursor.execute(f"SELECT a FROM {table_name} ORDER BY a")
            results = cursor.fetchallnumpy()
            expected = MaskedArray([1, 2, 3, 4, 5], mask=False)
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_batch_fetch(dsn, configuration):
    with open_cursor(configuration, rows_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.executemany(
                f"INSERT INTO {table_name} VALUES (?)",
                [[1], [2], [3], [4], [5]],
            )
            cursor.execute(f"SELECT a FROM {table_name} ORDER BY a")
            batches = cursor.fetchnumpybatches()
            expected_batches = [
                MaskedArray([1, 2], mask=False),
                MaskedArray([3, 4], mask=False),
                MaskedArray([5], mask=False),
            ]
            for idx, batch in enumerate(batches):
                expected = expected_batches[idx]
                assert_equal(batch[_fix_case(configuration, "a")], expected)
            assert_equal(idx, 2)


@for_each_database
def test_numpy_timestamp_column(dsn, configuration):
    supported_digits = configuration["capabilities"]["fractional_second_digits"]
    fractional = generate_microseconds_with_precision(supported_digits)
    timestamp = datetime.datetime(2015, 12, 31, 1, 2, 3, fractional)

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT TIMESTAMP") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", [timestamp])
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            expected = MaskedArray([timestamp], mask=[0], dtype="datetime64[us]")
            assert results[_fix_case(configuration, "a")].dtype == numpy.dtype(
                "datetime64[us]"
            )
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_date_column(dsn, configuration):
    date = datetime.date(3999, 12, 31)

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT DATE") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", [date])
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            expected = MaskedArray([date], mask=[0], dtype="datetime64[D]")
            assert results[_fix_case(configuration, "a")].dtype == numpy.dtype(
                "datetime64[D]"
            )
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_timelike_column_with_null(dsn, configuration):
    fill_value = 0

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT TIMESTAMP") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", [None])
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            expected = MaskedArray([42], mask=[1], dtype="datetime64[us]")
            assert_equal(
                results[_fix_case(configuration, "a")].filled(fill_value),
                expected.filled(fill_value),
            )


@for_each_database
def test_numpy_timelike_column_larger_than_batch_size(dsn, configuration):
    timestamps = [
        datetime.datetime(2015, 12, 31, 1, 2, 3),
        datetime.datetime(2016, 1, 5, 4, 5, 6),
        datetime.datetime(2017, 2, 6, 7, 8, 9),
        datetime.datetime(2018, 3, 7, 10, 11, 12),
        datetime.datetime(2019, 4, 8, 13, 14, 15),
    ]

    with open_cursor(configuration, rows_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, "INSERT TIMESTAMP") as table_name:
            cursor.executemany(
                f"INSERT INTO {table_name} VALUES (?)",
                [[timestamp] for timestamp in timestamps],
            )
            cursor.execute(f"SELECT a FROM {table_name} ORDER BY a")
            results = cursor.fetchallnumpy()
            expected = MaskedArray(timestamps, mask=[0], dtype="datetime64[us]")
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_string_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT STRING") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", ["this is a test"])
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            expected = MaskedArray(["this is a test"], mask=[0], dtype=numpy.object_)
            assert results[_fix_case(configuration, "a")].dtype == numpy.object_
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_string_column_with_truncation(dsn, configuration):
    with open_cursor(
        configuration, varchar_max_character_limit=9, limit_varchar_results_to_max=True
    ) as cursor:
        with query_fixture(cursor, configuration, "INSERT STRING MAX") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", ["truncated string"])
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            expected = MaskedArray(["truncated"], mask=[0], dtype=numpy.object_)
            assert results[_fix_case(configuration, "a")].dtype == numpy.object_
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_unicode_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT UNICODE") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", ["unicode \u2665"])
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            expected = MaskedArray(["unicode \u2665"], mask=[0], dtype=numpy.object_)
            assert results[_fix_case(configuration, "a")].dtype == numpy.object_
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_unicode_column_with_truncation(dsn, configuration):
    with open_cursor(
        configuration,
        rows_to_buffer=1,
        varchar_max_character_limit=9,
        limit_varchar_results_to_max=True,
    ) as cursor:
        with query_fixture(cursor, configuration, "INSERT UNICODE MAX") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", ["truncated string"])
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            expected = MaskedArray(["truncated"], mask=[0], dtype=numpy.object_)
            assert results[_fix_case(configuration, "a")].dtype == numpy.object_
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_string_column_with_null(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT STRING") as table_name:
            cursor.execute(f"INSERT INTO {table_name} VALUES (?)", [None])
            cursor.execute(f"SELECT a FROM {table_name}")
            results = cursor.fetchallnumpy()
            expected = MaskedArray([None], mask=[0], dtype=numpy.object_)
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_string_column_larger_than_batch_size(dsn, configuration):
    strings = ["abc", "def", "ghi", "jkl", "mno"]
    with open_cursor(configuration, rows_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, "INSERT STRING") as table_name:
            cursor.executemany(
                f"INSERT INTO {table_name} VALUES (?)",
                [[string] for string in strings],
            )
            cursor.execute(f"SELECT a FROM {table_name} ORDER BY a")
            results = cursor.fetchallnumpy()
            expected = MaskedArray(strings, mask=[0], dtype=numpy.object_)
            assert_equal(results[_fix_case(configuration, "a")], expected)


@for_each_database
def test_numpy_two_columns(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(
            cursor, configuration, "INSERT TWO INTEGER COLUMNS"
        ) as table_name:
            cursor.executemany(
                f"INSERT INTO {table_name} VALUES (?, ?)", [[1, 42], [2, 41]]
            )
            cursor.execute(f"SELECT a, b FROM {table_name} ORDER BY a")
            results = cursor.fetchallnumpy()
            assert_equal(
                results[_fix_case(configuration, "a")], MaskedArray([1, 2], mask=False)
            )
            assert_equal(
                results[_fix_case(configuration, "b")],
                MaskedArray([42, 41], mask=False),
            )
