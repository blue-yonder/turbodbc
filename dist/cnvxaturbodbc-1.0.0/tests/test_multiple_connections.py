from helpers import for_one_database, open_cursor


@for_one_database
def test_multiple_open_connections(dsn, configuration):
    with open_cursor(configuration) as cursor_1:
        assert cursor_1.executemany("SELECT 42").fetchall() == [[42]]

        with open_cursor(configuration) as cursor_2:
            cursor_2.executemany("SELECT 2")
            cursor_1.executemany("SELECT 1")
            assert cursor_2.fetchall() == [[2]]
            assert cursor_1.fetchall() == [[1]]

        assert cursor_1.executemany("SELECT 123").fetchall() == [[123]]
