import pytest
from helpers import for_one_database, for_specific_databases, get_credentials

from turbodbc import DatabaseError, ParameterError, connect
from turbodbc.connect import _make_connection_string
from turbodbc.connection import Connection


def _test_connection_string(expected, actual):
    assert len(expected) == len(actual)
    expected_tokens = expected.split(";")
    actual_tokens = actual.split(";")
    assert len(expected_tokens) == len(actual_tokens)

    for token in expected_tokens:
        assert token in actual_tokens


def test_make_connection_string_with_dsn():
    connection_string = _make_connection_string("my_dsn", user="my_user")
    _test_connection_string(connection_string, "dsn=my_dsn;user=my_user")


def test_make_connection_string_without_dsn():
    connection_string = _make_connection_string(None, user="my_user")
    _test_connection_string(connection_string, "user=my_user")


@for_one_database
def test_connect_returns_connection_when_successful(dsn, configuration):
    connection = connect(dsn, **get_credentials(configuration))
    assert isinstance(connection, Connection)


@for_one_database
def test_connect_returns_connection_with_explicit_dsn(dsn, configuration):
    connection = connect(dsn=dsn, **get_credentials(configuration))
    assert isinstance(connection, Connection)


def test_connect_raises_on_invalid_dsn():
    invalid_dsn = "This data source does not exist"
    with pytest.raises(DatabaseError):
        connect(invalid_dsn)


@for_specific_databases("postgres")
def test_connect_raises_on_invalid_additional_option(dsn, configuration):
    additional_option = {
        configuration["capabilities"]["connection_user_option"]: "invalid user"
    }
    with pytest.raises(DatabaseError):
        connect(dsn=dsn, **additional_option)


def test_connect_raises_on_ambiguous_parameters():
    with pytest.raises(ParameterError):
        connect("foo", connection_string="DRIVER=bar;SERVER=baz;")
    with pytest.raises(ParameterError):
        connect(connection_string="DRIVER=foo;SERVER=bar;", baz="qux")


@for_one_database
def test_connect_with_connection_string(dsn, configuration):
    connection_string = "DSN=%s;" % dsn
    for para, val in get_credentials(configuration).items():
        connection_string = connection_string + f"{para}={val};"  # noqa
    connection = connect(connection_string=connection_string)
    connection.cursor().execute("SELECT 'foo'")
    connection.close()
