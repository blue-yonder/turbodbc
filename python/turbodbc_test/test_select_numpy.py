import datetime
from collections import OrderedDict

from numpy.ma import MaskedArray
from numpy.testing import assert_equal
import numpy
import pytest
from mock import patch

import turbodbc

from turbodbc.cursor import _has_numpy_support
from query_fixture import query_fixture
from helpers import open_cursor, for_each_database, for_one_database, generate_microseconds_with_precision


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
def test_no_numpy_support(dsn, configuration):
    with open_cursor(configuration) as cursor:
        cursor.execute("SELECT 42")
        with patch('turbodbc.cursor._has_numpy_support', return_value=False):
            with pytest.raises(turbodbc.Error):
                cursor.fetchallnumpy()


@for_one_database
def test_numpy_without_result_set_raises(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with pytest.raises(turbodbc.InterfaceError):
            cursor.fetchallnumpy()


@for_each_database
def test_numpy_empty_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT INTEGER') as table_name:
            cursor.execute("SELECT a FROM {}".format(table_name))
            results = cursor.fetchallnumpy()
            assert isinstance(results, OrderedDict)
            assert len(results) == 1
            assert isinstance(results[_fix_case(configuration, 'a')], MaskedArray)


@for_each_database
def test_numpy_int_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        cursor.execute("SELECT 42 AS a")
        results = cursor.fetchallnumpy()
        expected = MaskedArray([42], mask=[0])
        assert results[_fix_case(configuration, 'a')].dtype == numpy.int64
        assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_double_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'SELECT DOUBLE') as query:
            cursor.execute(query)
            results = cursor.fetchallnumpy()
            expected = MaskedArray([3.14], mask=[0])
            assert results[_fix_case(configuration, 'a')].dtype == numpy.float64
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_boolean_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT INDEXED BOOL') as table_name:
            cursor.executemany('INSERT INTO {} VALUES (?, ?)'.format(table_name),
                                [[True, 1], [False, 2], [True, 3]])
            cursor.execute('SELECT a FROM {} ORDER BY b'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([True, False, True], mask=[0])
            assert results[_fix_case(configuration, 'a')].dtype == numpy.bool_
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_binary_column_with_null(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TWO INTEGER COLUMNS') as table_name:
            cursor.executemany("INSERT INTO {} VALUES (?, ?)".format(table_name),
                               [[42, 1], [None, 2]]) # second column to enforce ordering
            cursor.execute("SELECT a FROM {} ORDER BY b".format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([42, 0], mask=[0, 1])
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_binary_column_larger_than_batch_size(dsn, configuration):
    with open_cursor(configuration, rows_to_buffer=2) as cursor:
        with query_fixture(cursor, configuration, 'INSERT INTEGER') as table_name:
            cursor.executemany("INSERT INTO {} VALUES (?)".format(table_name),
                               [[1], [2], [3], [4], [5]])
            cursor.execute("SELECT a FROM {} ORDER BY a".format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([1, 2, 3, 4, 5], mask=False)
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_timestamp_column(dsn, configuration):
    supported_digits = configuration['capabilities']['fractional_second_digits']
    fractional = generate_microseconds_with_precision(supported_digits)
    timestamp = datetime.datetime(2015, 12, 31, 1, 2, 3, fractional)

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TIMESTAMP') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [timestamp])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([timestamp], mask=[0], dtype='datetime64[us]')
            assert results[_fix_case(configuration, 'a')].dtype == numpy.dtype('datetime64[us]')
            assert_equal(results[_fix_case(configuration, 'a')], expected)

@for_each_database
def test_numpy_timestamp_column_pre1400(dsn, configuration):
    supported_digits = configuration['capabilities']['fractional_second_digits']
    fractional = generate_microseconds_with_precision(supported_digits)
    timestamp = datetime.datetime(400, 12, 31, 1, 2, 3, fractional)

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TIMESTAMP') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [timestamp])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([timestamp], mask=[0], dtype='datetime64[us]')
            assert results[_fix_case(configuration, 'a')].dtype == numpy.dtype('datetime64[us]')
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_date_column(dsn, configuration):
    date = datetime.date(3999, 12, 31)

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT DATE') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [date])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([date], mask=[0], dtype='datetime64[D]')
            assert results[_fix_case(configuration, 'a')].dtype == numpy.dtype('datetime64[D]')
            assert_equal(results[_fix_case(configuration, 'a')], expected)

@for_each_database
def test_numpy_date_column_pre1400(dsn, configuration):
    date = datetime.date(400, 12, 31)

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT DATE') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [date])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([date], mask=[0], dtype='datetime64[D]')
            assert results[_fix_case(configuration, 'a')].dtype == numpy.dtype('datetime64[D]')
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_timelike_column_with_null(dsn, configuration):
    fill_value = 0;

    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TIMESTAMP') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [None])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([42], mask=[1], dtype='datetime64[us]')
            assert_equal(results[_fix_case(configuration, 'a')].filled(fill_value),
                         expected.filled(fill_value))


@for_each_database
def test_numpy_timelike_column_larger_than_batch_size(dsn, configuration):
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
            results = cursor.fetchallnumpy()
            expected = MaskedArray(timestamps, mask=[0], dtype='datetime64[us]')
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_string_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT STRING') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [u'this is a test'])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([u'this is a test'], mask=[0], dtype=numpy.object_)
            assert results[_fix_case(configuration, 'a')].dtype == numpy.object_
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_unicode_column(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT UNICODE') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [u'unicode \u2665'])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([u'unicode \u2665'], mask=[0], dtype=numpy.object_)
            assert results[_fix_case(configuration, 'a')].dtype == numpy.object_
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_string_column_with_null(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT STRING') as table_name:
            cursor.execute('INSERT INTO {} VALUES (?)'.format(table_name), [None])
            cursor.execute('SELECT a FROM {}'.format(table_name))
            results = cursor.fetchallnumpy()
            expected = MaskedArray([None], mask=[0], dtype=numpy.object_)
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_string_column_larger_than_batch_size(dsn, configuration):
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
            results = cursor.fetchallnumpy()
            expected = MaskedArray(strings, mask=[0], dtype=numpy.object_)
            assert_equal(results[_fix_case(configuration, 'a')], expected)


@for_each_database
def test_numpy_two_columns(dsn, configuration):
    with open_cursor(configuration) as cursor:
        with query_fixture(cursor, configuration, 'INSERT TWO INTEGER COLUMNS') as table_name:
            cursor.executemany("INSERT INTO {} VALUES (?, ?)".format(table_name),
                               [[1, 42], [2, 41]])
            cursor.execute("SELECT a, b FROM {} ORDER BY a".format(table_name))
            results = cursor.fetchallnumpy()
            assert_equal(results[_fix_case(configuration, 'a')], MaskedArray([1, 2], mask=False))
            assert_equal(results[_fix_case(configuration, 'b')], MaskedArray([42, 41], mask=False))
