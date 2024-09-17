import pytest
from helpers import for_one_database, open_cursor
from numpy import array
from numpy.ma import MaskedArray
from query_fixture import query_fixture

import turbodbc
from turbodbc import InterfaceError

try:
    import pyarrow as pa
    import turbodbc_arrow_support  # noqa: F401

    HAVE_ARROW = True
except ImportError:
    HAVE_ARROW = False


arrow_support = pytest.mark.skipif(
    not HAVE_ARROW, reason="not build with Arrow support"
)


@for_one_database
def test_column_of_unsupported_type_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            columns = ["this is not a NumPy MaskedArray"]
            with pytest.raises(turbodbc.InterfaceError):
                cursor.executemanycolumns(
                    f"INSERT INTO {table_name} VALUES (?)", columns
                )


@for_one_database
def test_columns_of_unequal_sizes_raise(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            columns = [
                MaskedArray([1, 2, 3], mask=False, dtype="int64"),
                MaskedArray([1, 2], mask=False, dtype="int64"),
            ]
            with pytest.raises(turbodbc.InterfaceError):
                cursor.executemanycolumns(
                    f"INSERT INTO {table_name} VALUES (?)", columns
                )


@for_one_database
def test_column_with_incompatible_dtype_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            columns = [MaskedArray([1, 2, 3], mask=False, dtype="int16")]
            with pytest.raises(turbodbc.InterfaceError):
                cursor.executemanycolumns(
                    f"INSERT INTO {table_name} VALUES (?)", columns
                )


@for_one_database
def test_column_with_multiple_dimensions_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            two_dimensional = array([[1, 2, 3], [4, 5, 6]], dtype="int64")
            columns = [two_dimensional]
            with pytest.raises(turbodbc.InterfaceError):
                cursor.executemanycolumns(
                    f"INSERT INTO {table_name} VALUES (?)", columns
                )


@for_one_database
def test_column_with_non_contiguous_data_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            two_dimensional = array([[1, 2, 3], [4, 5, 6]], dtype="int64")
            one_dimensional = two_dimensional[:, 1]
            assert not one_dimensional.flags.c_contiguous
            columns = [one_dimensional]
            with pytest.raises(turbodbc.InterfaceError):
                cursor.executemanycolumns(
                    f"INSERT INTO {table_name} VALUES (?)", columns
                )


@for_one_database
def test_number_of_columns_does_not_match_parameter_count(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            columns = [array([42], dtype="int64"), array([17], dtype="int64")]
            with pytest.raises(turbodbc.InterfaceError):
                cursor.executemanycolumns(
                    f"INSERT INTO {table_name} VALUES (?)", columns
                )


@for_one_database
def test_passing_empty_list_of_columns_is_ok(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (42)", [])

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == [[42]]


@for_one_database
def test_passing_empty_column_is_ok(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            columns = [array([], dtype="int64")]
            cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", columns)

            results = cursor.execute(
                f"SELECT A FROM {table_name} ORDER BY A"
            ).fetchall()
            assert results == []


@arrow_support
@for_one_database
def test_arrow_table_exceeds_expected_columns(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(
            cursor, configuration, "INSERT TWO INTEGER COLUMNS"
        ) as table_name:
            columns = [
                array([17, 23, 42], dtype="int64"),
                array([3, 2, 1], dtype="int64"),
                array([17, 23, 42], dtype="int64"),
            ]
            columns = [pa.Array.from_pandas(x) for x in columns]
            columns = pa.Table.from_arrays(columns, ["column1", "column2", "column3"])
            # InterfaceError: Number of passed columns (3) is not equal to the number of parameters (2)
            with pytest.raises(InterfaceError):
                cursor.executemanycolumns(
                    f"INSERT INTO {table_name} VALUES (?, ?)", columns
                )


@arrow_support
@for_one_database
def test_arrow_table_chunked_arrays_not_supported(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
            arr = pa.array([1, 2])
            rb = pa.RecordBatch.from_arrays([arr], ["a"])
            table = pa.Table.from_batches([rb, rb])
            with pytest.raises(NotImplementedError):
                cursor.executemanycolumns(f"INSERT INTO {table_name} VALUES (?)", table)
