import pytest

from turbodbc import connect, InterfaceError, DatabaseError, make_options

from helpers import for_one_database, get_credentials
from query_fixture import unique_table_name


@for_one_database
def test_cursor_on_closed_connection_raises(dsn, configuration):
    connection = connect(dsn, **get_credentials(configuration))
    connection.close()

    with pytest.raises(InterfaceError):
        connection.cursor()


@for_one_database
def test_closing_twice_is_ok(dsn, configuration):
    connection = connect(dsn, **get_credentials(configuration))

    connection.close()
    connection.close()


@for_one_database
def test_closing_connection_closes_all_cursors(dsn, configuration):
    connection = connect(dsn, **get_credentials(configuration))
    cursor_1 = connection.cursor()
    cursor_2 = connection.cursor()
    connection.close()

    with pytest.raises(InterfaceError):
        cursor_1.execute("SELECT 42")

    with pytest.raises(InterfaceError):
        cursor_2.execute("SELECT 42")


@for_one_database
def test_no_autocommit(dsn, configuration):
    connection = connect(dsn, **get_credentials(configuration))

    connection.cursor().execute('CREATE TABLE test_no_autocommit (a INTEGER)')
    connection.close()

    connection = connect(dsn, **get_credentials(configuration))
    with pytest.raises(DatabaseError):
        connection.cursor().execute('SELECT * FROM test_no_autocommit')


@for_one_database
def test_commit_on_closed_connection_raises(dsn, configuration):
    connection = connect(dsn, **get_credentials(configuration))
    connection.close()

    with pytest.raises(InterfaceError):
        connection.commit()


@for_one_database
def test_commit(dsn, configuration):
    table_name = unique_table_name()
    connection = connect(dsn, **get_credentials(configuration))

    connection.cursor().execute('CREATE TABLE {} (a INTEGER)'.format(table_name))
    connection.commit()

    connection.close()

    connection = connect(dsn, **get_credentials(configuration))
    cursor = connection.cursor()
    cursor.execute('SELECT * FROM {}'.format(table_name))
    results = cursor.fetchall()
    assert results == []
    
    cursor.execute('DROP TABLE {}'.format(table_name))
    connection.commit()


@for_one_database
def test_rollback_on_closed_connection_raises(dsn, configuration):
    connection = connect(dsn, **get_credentials(configuration))
    connection.close()

    with pytest.raises(InterfaceError):
        connection.rollback()


@for_one_database
def test_rollback(dsn, configuration):
    table_name = unique_table_name()
    connection = connect(dsn, **get_credentials(configuration))

    connection.cursor().execute('CREATE TABLE {} (a INTEGER)'.format(table_name))
    connection.rollback()

    with pytest.raises(DatabaseError):
        connection.cursor().execute('SELECT * FROM {}'.format(table_name))


@for_one_database
def test_autocommit_enabled_at_start(dsn, configuration):
    table_name = unique_table_name()
    options = make_options(autocommit=True)
    connection = connect(dsn, turbodbc_options=options, **get_credentials(configuration))

    connection.cursor().execute('CREATE TABLE {} (a INTEGER)'.format(table_name))
    connection.close()

    connection = connect(dsn, **get_credentials(configuration))
    cursor = connection.cursor()
    cursor.execute('SELECT * FROM {}'.format(table_name))
    results = cursor.fetchall()
    assert results == []

    cursor.execute('DROP TABLE {}'.format(table_name))
    connection.commit()


@for_one_database
def test_autocommit_switching(dsn, configuration):
    table_name = unique_table_name()

    connection = connect(dsn, **get_credentials(configuration))
    connection.autocommit = True   # <---
    connection.cursor().execute('CREATE TABLE {} (a INTEGER)'.format(table_name))
    connection.close()

    options = make_options(autocommit=True)
    connection = connect(dsn, turbodbc_options=options, **get_credentials(configuration))
    connection.autocommit = False  # <---
    connection.cursor().execute('INSERT INTO {} VALUES (?)'.format(table_name), [42])
    connection.close()

    # table is there, but data was not persisted
    connection = connect(dsn, **get_credentials(configuration))
    cursor = connection.cursor()
    cursor.execute('SELECT * FROM {}'.format(table_name))
    results = cursor.fetchall()
    assert results == []

    cursor.execute('DROP TABLE {}'.format(table_name))
    connection.commit()


@for_one_database
def test_autocommit_querying(dsn, configuration):
    connection = connect(dsn, **get_credentials(configuration))
    assert connection.autocommit == False
    connection.autocommit = True
    assert connection.autocommit == True
    connection.close()

@for_one_database
def test_pep343_with_statement(dsn, configuration):

    with connect(dsn, **get_credentials(configuration)) connection:
        cursor = connection.cursor()

    # connection should be closed, test it with the cursor
    with pytest.raises(InterfaceError):
        cursor.execute("SELECT 42")
