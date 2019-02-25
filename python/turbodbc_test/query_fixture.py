from contextlib import contextmanager

import random


def unique_table_name():
    return 'test_{}'.format(random.randint(0, 1000000000))

@contextmanager
def query_fixture(cursor, configuration, fixture_key, ignore_drop_errors=False):
    """
    Context manager used to set up fixtures for setting up queries.
    :param cursor: This cursor is used to execute queries
    :param configuration: A dictionary containing configuration and fixtures
    :param fixture_key: Identifies the fixture
    
    The context manager performs the following tasks:
    * If present, create a table or view based on what is written in the
      fixture's "table" or "view" key
    * If present, execute all queries listed in the fixture's "setup" section
    * If present, return the query listed in the fixture's "payload" section;
      else return the name of a temporary table
    * Clean up any tables or views created. If ignore_drop_errors is set, ignore
      exceptions during cleanup. This can be sueful if the tests make the cursor
      unusable for the cleanup statements.
    
    The fixtures dictionary should have the following format:
    
    {
        "setup": {
            "view": {
                "create": ["CREATE OR REPLACE VIEW {table_name} AS {content}"],
                "drop": ["DROP VIEW {table_name}"]
            },
            "table": {
                "create": ["CREATE OR REPLACE TABLE {table_name} ({content})"],
                "drop": ["DROP TABLE {table_name}"]
            }
        },
        "fixtures": {
            "my_fixture_key_with_a_table": {
                "table": "field1 INTEGER, field2 VARCHAR(10)",
                "setup": ["A POTENTIALLY EMPTY LIST OF ADDITIONAL SQL QUERIES"],
                "payload": "SELECT 42"
            }
            "my_fixture_key_with_a_view": {
                "view": "SELECT 42",
                "setup": ["A POTENTIALLY EMPTY LIST OF ADDITIONAL SQL QUERIES"],
            }

        }
    }
    
    All sections are optional. Queries may contain
    "{table_name}" to be replaced with a random table (or view) name.
    """
    fixture = configuration['queries'][fixture_key]
    table_name = unique_table_name()
    
    def _execute_queries(queries, replacements):
        if not isinstance(queries, list):
            queries = [queries]

        for query in queries:
            try:
                cursor.execute(query.format(**replacements))
            except Exception as error:
                raise type(error)('Error executing query "{}": {}'.format(query, error))

    def create_objects():
        if 'view' in fixture:
            queries = configuration['setup']['view']['create']
            replacements = {'table_name': table_name,
                            'content': fixture['view']}
            _execute_queries(queries, replacements)
        if 'table' in fixture:
            queries = configuration['setup']['table']['create']
            replacements = {'table_name': table_name,
                            'content': fixture['table']}
            _execute_queries(queries, replacements)

    def drop_objects():
        if 'view' in fixture:
            queries = configuration['setup']['view']['drop']
            replacements = {'table_name': table_name}
            _execute_queries(queries, replacements)
        if 'table' in fixture:
            queries = configuration['setup']['table']['drop']
            replacements = {'table_name': table_name}
            _execute_queries(queries, replacements)

    create_objects()
    try:
        if 'setup' in fixture:
            replacements = {'table_name': table_name}
            _execute_queries(fixture['setup'], replacements)
        if 'payload' in fixture:
            yield fixture['payload'].format(table_name=table_name)
        else:
            yield table_name
    finally:
        try:
            drop_objects()
        except Exception:
            if not ignore_drop_errors:
                raise
