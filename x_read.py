import turbodbc
from time import perf_counter

from pathlib import Path
import sys


def smoke():
    # Leaves a sign and allows us to attach to the process
    smoke_file = Path("smoke_hey_im_here")
    smoke_file.touch(exist_ok=True)
    with open(smoke_file):
        input("Press enter to continue after attaching to the process")



def turbodbc_read_sql(
    query: str, turbodbc_connection: turbodbc.connection.Connection
):
    with turbodbc_connection.cursor() as cursor:
        cursor.execute(query)
        return cursor.fetchallarrow().to_pandas()


def mssql_connect_turbodbc() -> turbodbc.connection.Connection:
    odbc_string = Path("./x_conn_str_odbc.txt").read_text(encoding="utf-8").strip("\n")
    conn_string_components = {
        x.split("=")[0].lower(): x.split("=")[1] for x in odbc_string.split(";")
    }
    print(conn_string_components)
    return turbodbc.connect(
        **conn_string_components,
        turbodbc_options=turbodbc.make_options(
            prefer_unicode=True, large_decimals_as_64_bit_types=True
        ),
    )


# smoke()


which = input("Which query: ") if len(sys.argv) < 2 else sys.argv[-1]
query = (
    Path("./query_ops.sql").read_text(encoding="utf-8") if which == "ops" else
    Path("./query_icd.sql").read_text(encoding="utf-8") if which == "icd" else
    f"SELECT * FROM {which}"
)
if not query:
    raise ValueError(which)
print("Using", query)

connection = mssql_connect_turbodbc()
start = perf_counter()
res = turbodbc_read_sql(query, connection)
print("Elapsed seconds:", perf_counter() - start)

# tag = input("Output tag: ")
# res.to_parquet(f"./{which}_{tag}.parquet")
