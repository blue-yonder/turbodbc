from helpers import (
    for_each_database_except,
    for_one_database,
    for_specific_databases,
    get_credentials,
)

from turbodbc import connect


@for_one_database
def test_nextset_supported(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    assert "nextset" in dir(cursor)


@for_one_database
def test_nextset_with_one_result_set(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    cursor.execute("SELECT 42")
    try:
        cursor.nextset()
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_specific_databases("postgres")
def test_nextset_with_function(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_func = """CREATE FUNCTION TEST_FUNC ()
    RETURNS SETOF int AS
    $func$
    BEGIN
    RETURN QUERY SELECT 4;
    RETURN QUERY SELECT 2;
    END
    $func$ LANGUAGE plpgsql;
    """
    cursor.execute(multi_result_set_func)
    cursor.execute("select TEST_FUNC();")
    try:
        assert cursor.fetchall() == [[4], [2]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_specific_databases("postgres")
def test_nextset_with_postgres_procedure(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_func = """CREATE PROCEDURE TEST_PROC(INOUT _result_one
    REFCURSOR = 'result_one', INOUT _result_two REFCURSOR = 'result_two')
    LANGUAGE 'plpgsql'
    AS $BODY$ BEGIN
        OPEN _result_one FOR SELECT 4;

        OPEN _result_two FOR SELECT 2;
    END $BODY$;
    """
    cursor.execute(multi_result_set_func)
    multi_result_call_func = """
    CALL TEST_PROC();
    BEGIN;
        FETCH ALL FROM "result_one";
        FETCH ALL FROM "result_two";
    COMMIT;
    """
    cursor.execute(multi_result_call_func)
    try:
        assert cursor.fetchall() == [["result_one", "result_two"]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_specific_databases("postgres")
def test_nextset_with_postgres_function(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_func = """CREATE FUNCTION TEST_FUNC() RETURNS SETOF refcursor as $$
    DECLARE
        result_one refcursor := 'result_one';
        result_two refcursor := 'result_two';
    BEGIN
        OPEN result_one FOR SELECT 4;
        RETURN NEXT result_one;
        OPEN result_two FOR SELECT 2;
        RETURN NEXT result_two;
    END;
    $$ LANGUAGE plpgsql;
    """
    cursor.execute(multi_result_set_func)
    multi_result_call_func = """
    select * from TEST_FUNC();
    BEGIN;
        FETCH ALL FROM "result_one";
        FETCH ALL FROM "result_two";
    COMMIT;
    """
    cursor.execute(multi_result_call_func)
    try:
        assert cursor.fetchall() == [["result_one"], ["result_two"]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_one_database
def test_nextset_with_two_select_statements_postgres(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    cursor.execute("SELECT 4;SELECT 2;")
    try:
        assert cursor.fetchall() == [[4]]
        next_set_present = cursor.nextset()
        if next_set_present:
            assert cursor.fetchall() == [[2]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_each_database_except(["PostgreSQL", "MSSQL"])
def test_nextset_with_two_result_set_mysql(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_stored_proc = """
    CREATE PROCEDURE TEST_PROC_TWO_INTS()
    BEGIN

        SELECT 4;
        SELECT 2;

    END
    """
    cursor.execute(multi_result_set_stored_proc)
    cursor.execute("CALL TEST_PROC_TWO_INTS();")
    try:
        assert cursor.fetchall() == [[4]]
        next_set_present = cursor.nextset()
        if next_set_present:
            assert cursor.fetchall() == [[2]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_each_database_except(["PostgreSQL", "MSSQL"])
def test_nextset_with_two_result_set_with_alias_mysql(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_stored_proc = """
    CREATE PROCEDURE TEST_PROC_TWO_ALIAS_INTS()
    BEGIN

        SELECT 4 as Four;
        SELECT 2 as Two;

    END
    """
    cursor.execute(multi_result_set_stored_proc)
    cursor.execute("CALL TEST_PROC_TWO_ALIAS_INTS();")
    try:
        assert cursor.fetchall() == [[4]]
        next_set_present = cursor.nextset()
        if next_set_present:
            assert cursor.fetchall() == [[2]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_each_database_except(["PostgreSQL", "MSSQL"])
def test_nextset_with_three_result_set_mysql(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_stored_proc = """
    CREATE PROCEDURE TEST_PROC_THREE_INTS()
    BEGIN

        SELECT 4;
        SELECT 3;
        SELECT 2;

    END
    """
    cursor.execute(multi_result_set_stored_proc)
    cursor.execute("CALL TEST_PROC_THREE_INTS();")
    try:
        assert cursor.fetchall() == [[4]]
        next_set_present = cursor.nextset()
        if next_set_present:
            assert cursor.fetchall() == [[3]]
            next_set_present = cursor.nextset()
            if next_set_present:
                assert cursor.fetchall() == [[2]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_each_database_except(["PostgreSQL", "MySQL"])
def test_nextset_with_two_result_set_mssql(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_stored_proc = """CREATE PROCEDURE TEST_PROC
    AS
    SET NOCOUNT ON
    SELECT 4
    SELECT 2
    """
    cursor.execute(multi_result_set_stored_proc)
    cursor.execute("EXEC TEST_PROC;")
    try:
        assert cursor.fetchall() == [[4]]
        next_set_present = cursor.nextset()
        if next_set_present:
            assert cursor.fetchall() == [[2]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_each_database_except(["PostgreSQL", "MySQL"])
def test_nextset_with_two_result_set_with_alias_mssql(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_stored_proc = """CREATE PROCEDURE TEST_PROC
    AS
    SET NOCOUNT ON
    SELECT 4 as Four
    SELECT 2 as Two
    """
    cursor.execute(multi_result_set_stored_proc)
    cursor.execute("EXEC TEST_PROC;")
    try:
        assert cursor.fetchall() == [[4]]
        next_set_present = cursor.nextset()
        if next_set_present:
            assert cursor.fetchall() == [[2]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"


@for_each_database_except(["PostgreSQL", "MySQL"])
def test_nextset_with_three_result_set_mssql(dsn, configuration):
    cursor = connect(dsn, **get_credentials(configuration)).cursor()
    multi_result_set_stored_proc = """CREATE PROCEDURE TEST_PROC
    AS
    SET NOCOUNT ON
    SELECT 4
    SELECT 3
    SELECT 2
    """
    cursor.execute(multi_result_set_stored_proc)
    cursor.execute("EXEC TEST_PROC;")
    try:
        assert cursor.fetchall() == [[4]]
        next_set_present = cursor.nextset()
        if next_set_present:
            assert cursor.fetchall() == [[3]]
            next_set_present = cursor.nextset()
            if next_set_present:
                assert cursor.fetchall() == [[2]]
    except Exception as exc:
        assert False, f"Didn't find a call for nextset\n{exc}\n"
    else:
        assert True, "Found call for nextset"
