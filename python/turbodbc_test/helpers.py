from contextlib import contextmanager

import json
import os
import pytest

import turbodbc


def generate_microseconds_with_precision(digits):
    microseconds = 0;
    for i in range(digits):
        microseconds = 10 * microseconds + i + 1
    for i in range(6 - digits):
        microseconds *= 10

    return microseconds


def _get_config_files():
    variable = 'TURBODBC_TEST_CONFIGURATION_FILES'
    try:
        raw = os.environ[variable]
        file_names = raw.split(',')
        return [file_name.strip() for file_name in file_names]
    except KeyError:
        raise KeyError('Please set the environment variable {} to specify the configuration files as a comma-separated list'.format(variable))


def get_credentials(configuration):
    if 'user' in configuration:
        return {configuration['capabilities']['connection_user_option']: configuration['user'],
                configuration['capabilities']['connection_password_option']: configuration['password']}
    else:
        return {}

def _load_configuration(file_name):
    with open(file_name, 'r') as f:
        return json.load(f)


def _get_configuration(file_name):
    conf = _load_configuration(file_name)
    return (conf['data_source_name'], conf)


def _get_configurations():
    return [_get_configuration(file_name) for file_name in _get_config_files()]


"""
Use this decorator to execute a test function once for each database configuration.

Please note the test function *must* take the parameters `dsn` and `configuration`,
and in that order.

Example:

@for_each_database
def test_important_stuff(dsn, configuration):
    assert 1 == 2
"""
for_each_database = pytest.mark.parametrize("dsn,configuration",
                                            _get_configurations())



"""
Use this decorator to execute a test function once for a single database configuration.

Please note the test function *must* take the parameters `dsn` and `configuration`,
and in that order.

Example:

@for_one_database
def test_important_stuff(dsn, configuration):
    assert 1 == 2
"""
for_one_database = pytest.mark.parametrize("dsn,configuration",
                                           [_get_configuration(_get_config_files()[0])])



@contextmanager
def open_connection(configuration, rows_to_buffer=None, parameter_sets_to_buffer=100, use_async_io=False):
    dsn = configuration['data_source_name']
    prefer_unicode = configuration.get('prefer_unicode', False)
    read_buffer_size = turbodbc.Rows(rows_to_buffer) if rows_to_buffer else turbodbc.Megabytes(1)

    options = turbodbc.make_options(read_buffer_size=read_buffer_size,
                                    parameter_sets_to_buffer=parameter_sets_to_buffer,
                                    use_async_io=use_async_io,
                                    prefer_unicode=prefer_unicode)
    connection = turbodbc.connect(dsn, turbodbc_options=options, **get_credentials(configuration))

    yield connection
    connection.close()


@contextmanager
def open_cursor(configuration, rows_to_buffer=None, parameter_sets_to_buffer=100, use_async_io=False):
    with open_connection(configuration, rows_to_buffer, parameter_sets_to_buffer, use_async_io) as connection:
        cursor = connection.cursor()
        yield cursor
        cursor.close()
