import datetime

import pytest
from helpers import (
    for_each_database,
    for_one_database,
    generate_microseconds_with_precision,
    open_cursor,
)
from numpy import array
from numpy.ma import MaskedArray
from query_fixture import query_fixture

column_backends = ["numpy"]

try:
    import pyarrow as pa
    import turbodbc_arrow_support  # noqa: F401

    column_backends.append("arrow")
    # column_backends.append('pandas')
except ImportError:
    pass

for_each_column_backend = pytest.mark.parametrize("column_backend", column_backends)


def _to_columns(values, dtype, column_backend):
    if column_backend == "numpy":
        return [array(values, dtype=dtype)]
    elif column_backend == "arrow":
        columns = pa.Array.from_pandas(array(values, dtype=dtype))
        return pa.Table.from_arrays([columns], ["column"])


def _to_masked_columns(values, dtype, mask, column_backend):
    if column_backend == "numpy":
        return [MaskedArray(values, mask=mask, dtype=dtype)]
    elif column_backend == "arrow":
        columns = pa.array(array(values, dtype=dtype), mask=array(mask))
        return pa.Table.from_arrays([columns], ["column"])


def _test_basic_column(configuration, fixture, values, dtype, column_backend):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, fixture) as table_name:
            columns = _to_columns(values, dtype, column_backend)
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)
            assert cursor.rowcount == len(values)

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[value] for value in sorted(values)]


def _test_column_matches_buffer_size(
    configuration, fixture, values, dtype, column_backend
):
    with open_cursor(configuration, parameter_sets_to_buffer=len(values)) as cursor:
        with query_fixture(cursor, configuration, fixture) as table_name:
            columns = _to_columns(values, dtype, column_backend)
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)
            assert cursor.rowcount == len(values)

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[value] for value in sorted(values)]


def _test_column_exceeds_buffer_size(
    configuration, fixture, values, dtype, column_backend
):
    with open_cursor(configuration, parameter_sets_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, fixture) as table_name:
            columns = _to_columns(values, dtype, column_backend)
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)
            assert cursor.rowcount == len(values)

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[value] for value in sorted(values)]


def _test_masked_column(configuration, fixture, values, dtype, column_backend):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, fixture) as table_name:
            columns = _to_masked_columns(
                values, dtype, [False, True, False], column_backend
            )
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)
            assert cursor.rowcount == len(values)

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[value] for value in [values[0], values[2]]] + [
                [None]
            ] or results == [[None]] + [[value] for value in [values[0], values[2]]]


def _test_masked_column_with_shrunk_mask(configuration, fixture, values, dtype):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, fixture) as table_name:
            columns = [MaskedArray(values, mask=False, dtype=dtype)]
            columns[0].shrink_mask()
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)
            assert cursor.rowcount == len(values)

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[value] for value in sorted(values)]


def _test_masked_column_exceeds_buffer_size(
    configuration, fixture, values, dtype, column_backend
):
    with open_cursor(configuration, parameter_sets_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, fixture) as table_name:
            columns = _to_masked_columns(
                values, dtype, [True, False, True], column_backend
            )
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)
            assert cursor.rowcount == len(values)

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[values[1]], [None], [None]] or results == [
                [None],
                [None],
                [values[1]],
            ]


def _test_single_masked_value(configuration, fixture, values, dtype):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, fixture) as table_name:
            columns = [MaskedArray([values[0]], mask=[True], dtype=dtype)]
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)
            assert cursor.rowcount == 1

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[None]]


def _full_column_tests(configuration, fixture, values, dtype, column_backend):
    _test_basic_column(configuration, fixture, values, dtype, column_backend)
    _test_column_matches_buffer_size(
        configuration, fixture, values, dtype, column_backend
    )
    _test_column_exceeds_buffer_size(
        configuration, fixture, values, dtype, column_backend
    )
    _test_masked_column(configuration, fixture, values, dtype, column_backend)
    _test_masked_column_exceeds_buffer_size(
        configuration, fixture, values, dtype, column_backend
    )
    if column_backend == "numpy":
        # For these tests there is no equivalent input structure in Python
        _test_masked_column_with_shrunk_mask(configuration, fixture, values, dtype)
        _test_single_masked_value(configuration, fixture, values, dtype)


@for_each_column_backend
@for_each_database
@pytest.mark.parametrize(
    "dtype",
    [
        "int8",
        "int16",
        "int32",
        "int64",
        "uint8",
        "uint16",
        "uint32",
        "uint64",
    ],
)
def test_integer_column(dsn, configuration, column_backend, dtype):
    if column_backend == "numpy" and dtype != "int64":
        pytest.skip("numpy INSERTs only support int64 as integeral dtype")
    elif dtype == "uint64":
        pytest.skip("uint64 values may be too large to fit into int64")
    else:
        _full_column_tests(
            configuration, "INSERT INTEGER", [17, 23, 42], dtype, column_backend
        )


@for_each_column_backend
@for_each_database
def test_float64_column(dsn, configuration, column_backend):
    _full_column_tests(
        configuration, "INSERT DOUBLE", [2.71, 3.14, 6.25], "float64", column_backend
    )


@for_each_column_backend
@for_each_database
def test_datetime64_microseconds_column(dsn, configuration, column_backend):
    supported_digits = configuration["capabilities"]["fractional_second_digits"]
    fractional = generate_microseconds_with_precision(supported_digits)

    _full_column_tests(
        configuration,
        "INSERT TIMESTAMP",
        [
            datetime.datetime(2015, 12, 31, 1, 2, 3, fractional),
            datetime.datetime(2016, 1, 1, 4, 5, 6, fractional),
            datetime.datetime(2017, 5, 6, 7, 8, 9, fractional),
        ],
        "datetime64[us]",
        column_backend,
    )


@for_each_column_backend
@for_each_database
def test_datetime64_nanoseconds_column(dsn, configuration, column_backend):
    supported_digits = configuration["capabilities"]["fractional_second_digits"]
    # C++ unit test checks that conversion method is capable of nanosecond precision
    fractional = generate_microseconds_with_precision(supported_digits)

    _full_column_tests(
        configuration,
        "INSERT TIMESTAMP",
        [
            datetime.datetime(2015, 12, 31, 1, 2, 3, fractional),
            datetime.datetime(2016, 1, 1, 4, 5, 6, fractional),
            datetime.datetime(2017, 5, 6, 7, 8, 9, fractional),
        ],
        "datetime64[ns]",
        column_backend,
    )


@for_each_column_backend
@for_each_database
def test_datetime64_days_column(dsn, configuration, column_backend):
    _full_column_tests(
        configuration,
        "INSERT DATE",
        [
            datetime.date(2015, 12, 31),
            datetime.date(2016, 1, 1),
            datetime.date(2017, 5, 6),
        ],
        "datetime64[D]",
        column_backend,
    )


@for_each_column_backend
@for_each_database
def test_boolean_column(dsn, configuration, column_backend):
    _full_column_tests(
        configuration, "INSERT BOOL", [True, False, True], "bool", column_backend
    )


@for_each_column_backend
@for_each_database
def test_string_column(dsn, configuration, column_backend):
    _full_column_tests(
        configuration,
        "INSERT STRING",
        ["Simple", "Non-unicode", "Strings"],
        "object",
        column_backend,
    )


@for_each_column_backend
@for_each_database
def test_unicode_column(dsn, configuration, column_backend):
    _full_column_tests(
        configuration,
        "INSERT UNICODE",
        ["a\u2665\u2665\u2665\u2665\u2665", "b\u2665", "c\u2665\u2665\u2665"],
        "object",
        column_backend,
    )


def _test_none_in_string_column(configuration, fixture, column_backend):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, fixture) as table_name:
            columns = _to_columns([None], "object", column_backend)
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[None]]


@for_each_column_backend
@for_each_database
def test_string_column_with_None(dsn, configuration, column_backend):
    _test_none_in_string_column(configuration, "INSERT STRING", column_backend)


@for_each_column_backend
@for_each_database
def test_unicode_column_with_None(dsn, configuration, column_backend):
    _test_none_in_string_column(configuration, "INSERT UNICODE", column_backend)


@for_each_column_backend
@for_each_database
def test_multiple_columns(dsn, configuration, column_backend):
    with open_cursor(configuration) as cursor:
        with query_fixture(
            cursor, configuration, "INSERT TWO INTEGER COLUMNS"
        ) as table_name:
            columns = [
                array([17, 23, 42], dtype="int64"),
                array([3, 2, 1], dtype="int64"),
            ]
            if column_backend == "arrow":
                columns = [pa.Array.from_pandas(x) for x in columns]
                columns = pa.Table.from_arrays(columns, ["column1", "column2"])
            cursor.executemanycolumns(
                f"INSERT INTO {table_name} VALUES (?, ?)", columns
            )

            results = cursor.execute(
                f"SELECT A, B FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[17, 3], [23, 2], [42, 1]]


@for_one_database
def test_execute_many_columns_creates_result_set(dsn, configuration):
    with open_cursor(configuration) as cursor:
        cursor.executemanycolumns("SELECT 42", [])
        assert cursor.fetchall() == [[42]]


@for_one_database
def test_execute_many_columns_supports_chaining(dsn, configuration):
    with open_cursor(configuration) as cursor:
        rows = cursor.executemanycolumns("SELECT 42", []).fetchall()
        assert rows == [[42]]
