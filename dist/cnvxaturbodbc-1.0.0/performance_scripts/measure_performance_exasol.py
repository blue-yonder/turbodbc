import json
from datetime import date, datetime

import numpy
import pyodbc

import turbodbc


def connect(api, dsn):
    if api == "pyodbc":
        return pyodbc.connect(dsn=dsn)
    else:
        return turbodbc.connect(
            dsn,
            parameter_sets_to_buffer=100000,
            rows_to_buffer=100000,
            use_async_io=True,
        )


def _column_data(column_type):
    if column_type == "INTEGER":
        return 42
    if column_type == "DOUBLE":
        return 3.14
    if column_type == "DATE":
        return date(2016, 1, 2)
    if "VARCHAR" in column_type:
        return "test data"
    if column_type == "TIMESTAMP":
        return datetime(2016, 1, 2, 3, 4, 5)
    raise RuntimeError(f"Unknown column type {column_type}")


def prepare_test_data(cursor, column_types, powers_of_two_lines):
    columns = [
        f"col{i} {type}" for i, type in zip(range(len(column_types)), column_types)
    ]
    cursor.execute(
        "CREATE OR REPLACE TABLE test_performance ({})".format(", ".join(columns))
    )

    data = [_column_data(type) for type in column_types]
    cursor.execute(
        "INSERT INTO test_performance VALUES ({})".format(
            ", ".join("?" for _ in columns)
        ),
        data,
    )

    for _ in range(powers_of_two_lines):
        cursor.execute("INSERT INTO test_performance SELECT * FROM test_performance")


def _fetchallnumpy(cursor):
    cursor.fetchallnumpy()


def _stream_to_ignore(cursor):
    for _ in cursor:
        pass


def _stream_to_list(cursor):
    [row for row in cursor]


def measure(cursor, extraction_method):
    cursor.execute("SELECT * FROM test_performance")
    start = datetime.now()
    extraction_method(cursor)
    stop = datetime.now()
    return (stop - start).total_seconds()


powers_of_two = 21
n_rows = 2**powers_of_two
n_runs = 10
column_types = [
    "INTEGER"
]  # , 'INTEGER', 'DOUBLE', 'DOUBLE'] #, 'VARCHAR(20)', 'DATE', 'TIMESTAMP']
api = "pyodbc"
# extraction_method = _stream_to_ignore
extraction_method = _stream_to_list
# extraction_method = _fetchallnumpy
database = "Exasol"

connection = connect(api, database)
cursor = connection.cursor()

print(f"Performing benchmark with {n_rows} rows")
prepare_test_data(cursor, column_types, powers_of_two)

runs_list = []
for r in range(n_runs):
    print(f"Run #{r + 1}")
    runs_list.append(measure(cursor, extraction_method))

runs = numpy.array(runs_list)
results = {
    "number_of_runs": n_runs,
    "rows_per_run": n_rows,
    "column_types": column_types,
    "api": api,
    "extraction_method": extraction_method.__name__,
    "database": database,
    "timings": {
        "best": runs.min(),
        "worst": runs.max(),
        "mean": runs.mean(),
        "standard_deviation": runs.std(),
    },
    "rates": {
        "best": n_rows / runs.min(),
        "worst": n_rows / runs.max(),
        "mean": n_rows * numpy.reciprocal(runs).mean(),
        "standard_deviation": n_rows * numpy.reciprocal(runs).std(),
    },
}

print(json.dumps(results, indent=4, separators=(",", ": ")))
file_name = f"results_{database}_{api}{extraction_method.__name__}.json"
with open(file_name, "w") as file:
    json.dump(results, file, indent=4, separators=(",", ": "))

print(f"Wrote results to file {file_name}")
