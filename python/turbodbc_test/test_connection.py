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
    # Some database have transactional semantics also for DDL statement
    # (i.e. creating / deleting tables only takes effect after "commit").
    # However, other databases (most notably mysql) always create / delete tables,
    # even without a "commit". Therefore, this test is somewhat
    # more complex to work with both types of databases.
    connection = connect(dsn, **get_credentials(configuration))

    table_name = unique_table_name()
    
    cursor = connection.cursor()
    cursor.execute('CREATE TABLE {} (a INTEGER)'.format(table_name))
    cursor.execute('INSERT INTO {} VALUES (42)'.format(table_name))
    cursor.close()
    connection.close()

    connection = connect(dsn, **get_credentials(configuration))
    
    db_error = False
    result = [23]
    try:
        result = connection.cursor().execute('SELECT * FROM {}'.format(table_name)).fetchall()
    except DatabaseError:
        # databases with transactional DDL semantics enter here, because
        # they do not find {table_name}.
        db_error = True

    assert db_error or result == []
    if not db_error:
        connection.cursor().execute('DROP TABLE {}'.format(table_name))
        # no commit necessary ;-)


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
    connection.cursor().execute('INSERT INTO {} VALUES (37)'.format(table_name))
    connection.commit()

    connection.close()

    connection = connect(dsn, **get_credentials(configuration))
    cursor = connection.cursor()
    cursor.execute('SELECT * FROM {}'.format(table_name))
    results = cursor.fetchall()
    assert results == [[37]]
    
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
    # see comments in test_no_autocommit 
    table_name = unique_table_name()
    connection = connect(dsn, **get_credentials(configuration))

    cursor = connection.cursor()
    cursor.execute('CREATE TABLE {} (a INTEGER)'.format(table_name))
    cursor.execute('INSERT INTO {} VALUES (28)'.format(table_name))
    connection.rollback()

    db_error = False
    result = [23]
    try:
        result = connection.cursor().execute('SELECT * FROM {}'.format(table_name)).fetchall()
    except DatabaseError:
        # databases with transactional DDL semantics enter here, because
        # they do not find {table_name}.
        db_error = True
    assert db_error or result == []
    if not db_error:
        connection.cursor().execute('DROP TABLE {}'.format(table_name))
        # no commit necessary ;-)


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

    with connect(dsn, **get_credentials(configuration)) as connection:
        cursor = connection.cursor()

    # connection should be closed, test it with the cursor
    with pytest.raises(InterfaceError):
        cursor.execute("SELECT 42")
