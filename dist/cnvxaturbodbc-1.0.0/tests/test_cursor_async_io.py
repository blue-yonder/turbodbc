import pytest
from helpers import for_one_database  # , open_cursor

# from query_fixture import query_fixture


@for_one_database
def test_many_batches_with_async_io(dsn, configuration):
    pytest.skip("async_io will be removed in future")
    # with open_cursor(configuration, use_async_io=True) as cursor:
    #    with query_fixture(cursor, configuration, "INSERT INTEGER") as table_name:
    #        # insert 2^16 rows
    #        cursor.execute(f"INSERT INTO {table_name} VALUES (1)")
    #        for _ in range(16):
    #            cursor.execute(f"INSERT INTO {table_name} SELECT * FROM {table_name}")

    #        cursor.execute(f"SELECT * FROM {table_name}")
    #        assert sum(1 for _ in cursor) == 2**16
