from collections import OrderedDict
from mock import patch

import datetime
import gc
import pytest
import sys
import turbodbc

from query_fixture import query_fixture
from helpers import open_cursor, for_each_database, for_one_database, generate_microseconds_with_precision

# Skip all parquet tests if we can't import pyarrow.parquet
pa = pytest.importorskip('pyarrow')

# Ignore these with pytest ... -m 'not parquet'
pyarrow = pytest.mark.pyarrow


def _fix_case(configuration, string):
    """
    some databases return column names in upper case
    """
    capabilities = configuration['capabilities']
    if capabilities['reports_column_names_as_upper_case']:
        return string.upper()
    else:
        return string


@for_one_database
@pyarrow
def test_no_arrow_support(dsn, configuration):
    with open_cursor(configuration) as cursor:
        cursor.execute("SELECT 42")
        with patch('turbodbc.cursor._has_arrow_support', return_value=False):
            with pytest.raises(turbodbc.Error):
                cursor.fetchallarrow()


@for_one_database
@pyarrow
def test_arrow_without_result_set_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with pytest.raises(turbodbc.InterfaceError):
            cursor.fetchallarrow()


@for_each_database
@pyarrow
def test_arrow_empty_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT INTEGER') as table_name:
            cursor.execute("SELECT a FROM {}".format(table_name))
            result = cursor.fetchallarrow()
            assert isinstance(result, pa.Table)
            assert result.num_columns == 1
            assert result.num_rows == 0


@for_each_database
@pyarrow
def test_arrow_reference_count(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT INTEGER') as table_name:
            cursor.execute("SELECT a FROM {}".format(table_name))
            result = cursor.fetchallarrow()
            gc.collect()
            assert sys.getrefcount(result) == 2


@for_each_database
@pyarrow
def test_arrow_int_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        cursor.execute("SELECT 42 AS a")
        result = cursor.fetchallarrow()
        assert isinstance(result, pa.Table)
        assert result.num_columns == 1
        assert result.num_rows == 1
        assert result.column(0).name == _fix_case(configuration, "a")
        assert str(result.column(0).type) == "int64"
        assert result.column(0).to_pylist() == [42]


@for_each_database
@pyarrow
def test_arrow_int_column_adaptive(dsn, configuration):
    with open_cursor(configuration) as cursor:
        cursor.execute("SELECT 42 AS a")
        result = cursor.fetchallarrow(adaptive_integers=True)
        assert str(result.column(0).type) == "int8"
        assert result.column(0).to_pylist() == [42]


@for_each_database
@pyarrow
def test_arrow_double_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'SELECT DOUBLE') as query:
            cursor.execute(query)
            result = cursor.fetchallarrow()
            assert isinstance(result, pa.Table)
            assert result.num_columns == 1
            assert result.num_rows == 1
            assert result.column(0).name == _fix_case(configuration, "a")
            assert str(result.column(0).type) == "double"
            assert result.column(0).to_pylist() == [3.14]


@for_each_database
@pyarrow
def test_arrow_boolean_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT INDEXED BOOL') as table_name:
            cursor.executemany('INSERT INTO {} VALUES (?, ?)'.format(table_name),
                                [[True, 1], [False, 2], [True, 3]])
            cursor.execute('SELECT a FROM {} ORDER BY b'.format(table_name))
            result = cursor.fetchallarrow()
            assert isinstance(result, pa.Table)
            assert result.num_columns == 1
            assert result.num_rows == 3
            assert result.column(0).name == _fix_case(configuration, "a")
            assert str(result.column(0).type) == "bool"
            assert result.column(0).to_pylist() == [True, False, True]


@for_each_database
@pyarrow
def test_arrow_binary_column_with_null(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TWO INTEGER COLUMNS') as table_name:
            cursor.executemany("INSERT INTO {} VALUES (?, ?)".format(table_name),
                               [[42, 1], [None, 2]]) # second column to enforce ordering
            cursor.execute("SELECT a FROM {} ORDER BY b".format(table_name))
            result = cursor.fetchallarrow()
            assert isinstance(result, pa.Table)
            assert result.num_columns == 1
            assert result.num_rows == 2
            assert result.column(0).name == _fix_case(configuration, "a")
            assert str(result.column(0).type) == "int64"
            assert result.column(0).to_pylist() == [42, None]
            assert result.column(0).null_count == 1


@for_each_database
@pyarrow
def test_arrow_binary_column_larger_than_batch_size(dsn, configuration):
    with open_cursor(configuration, rows_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, 'INSERT INTEGER') as table_name:
            cursor.executemany("INSERT INTO {} VALUES (?)".format(table_name),
                               [[1], [2], [3], [4], [5]])
            cursor.execute("SELECT a FROM {} ORDER BY a".format(table_name))
            result = cursor.fetchallarrow()
            assert isinstance(result, pa.Table)
            assert result.column(0).to_pylist() == [1, 2, 3, 4, 5]


@for_each_database
@pyarrow
def test_arrow_timestamp_column(dsn, configuration):
    supported_digits = configuration['capabilities']['fractional_second_digits']
    fractional = generate_microseconds_with_precision(supported_digits)
    timestamp = datetime.datetime(2015, 12, 31, 1, 2, 3, fractional)

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TIMESTAMP') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [timestamp])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            result = cursor.fetchallarrow()
            assert result.column(0).to_pylist() == [timestamp]


@for_each_database
@pyarrow
def test_arrow_date_column(dsn, configuration):
    date = datetime.date(2015, 12, 31)

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT DATE') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [date])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            result = cursor.fetchallarrow()
            result.column(0).to_pylist() == [datetime.date(2015, 12, 31)]


@for_each_database
@pyarrow
def test_arrow_timelike_column_with_null(dsn, configuration):
    fill_value = 0;

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TIMESTAMP') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [None])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            result = cursor.fetchallarrow()
            assert result.column(0).to_pylist() == [None]


@for_each_database
@pyarrow
def test_arrow_timelike_column_larger_than_batch_size(dsn, configuration):
    timestamps = [datetime.datetime(2015, 12, 31, 1, 2, 3),
                  datetime.datetime(2016, 1, 5, 4, 5, 6),
                  datetime.datetime(2017, 2, 6, 7, 8, 9),
                  datetime.datetime(2018, 3, 7, 10, 11, 12),
                  datetime.datetime(2019, 4, 8, 13, 14, 15)]

    with open_cursor(configuration, rows_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TIMESTAMP') as table_name:
            cursor.executemany('INSERT INTO {} VALUES (?)'.format(table_name),
                               [[timestamp] for timestamp in timestamps])
            cursor.execute('SELECT a FROM {} ORDER BY a'.format(table_name))
            result = cursor.fetchallarrow()
            assert result.column(0).to_pylist() == timestamps


@for_each_database
@pyarrow
@pytest.mark.parametrize("strings_as_dictionary", [True, False])
def test_arrow_string_column(dsn, configuration, strings_as_dictionary):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT UNICODE') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [u'unicode \u2665'])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            result = cursor.fetchallarrow(strings_as_dictionary=strings_as_dictionary)
            assert result.column(0).to_pylist() == [u'unicode \u2665']


@for_each_database
@pyarrow
@pytest.mark.parametrize("strings_as_dictionary", [True, False])
def test_arrow_string_column_with_null(dsn, configuration, strings_as_dictionary):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT STRING') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [None])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            result = cursor.fetchallarrow(strings_as_dictionary=strings_as_dictionary)
            result.column(0).null_count == 1
            result.column(0).to_pylist() == [None]


@for_each_database
@pyarrow
@pytest.mark.parametrize("strings_as_dictionary", [True, False])
def test_arrow_string_column_larger_than_batch_size(dsn, configuration, strings_as_dictionary):
    strings = [u'abc',
               u'def',
               u'ghi',
               u'jkl',
               u'mno']
    with open_cursor(configuration, rows_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, 'INSERT STRING') as table_name:
            cursor.executemany('INSERT INTO {} VALUES (?)'.format(table_name),
                               [[string] for string in strings])
            cursor.execute('SELECT a FROM {} ORDER BY a'.format(table_name))
            result = cursor.fetchallarrow(strings_as_dictionary=strings_as_dictionary)
            result.column(0).to_pylist() == strings


@for_each_database
@pyarrow
def test_arrow_two_columns(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TWO INTEGER COLUMNS') as table_name:
            cursor.executemany("INSERT INTO {} VALUES (?, ?)".format(table_name),
                               [[1, 42], [2, 41]])
            cursor.execute("SELECT a, b FROM {} ORDER BY a".format(table_name))
            result = cursor.fetchallarrow()
            assert result.to_pydict() == OrderedDict([
                (_fix_case(configuration, 'a'), [1, 2]),
                (_fix_case(configuration, 'b'), [42, 41])]
            )


@for_each_database
@pyarrow
def test_arrow_two_columns(dsn, configuration):
    with open_cursor(configuration, rows_to_buffer=1) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TWO INTEGER COLUMNS') as table_name:
            cursor.executemany("INSERT INTO {} VALUES (?, ?)".format(table_name),
                               [[1, 42], [2, 41]])
            cursor.execute("SELECT a, b FROM {} ORDER BY a".format(table_name))
            result = list(cursor.fetcharrowbatches())
            assert len(result) == 2
