import turbodbc
from time import perf_counter


def turbodbc_read_sql(
    query: str, turbodbc_connection: turbodbc.connection.Connection
):
    with turbodbc_connection.cursor() as cursor:
        cursor.execute(query)
        return cursor.fetchallarrow().to_pandas()


def mssql_connect_turbodbc() -> turbodbc.connection.Connection:
    odbc_string = "Driver={libtdsodbc.so};Server=localhost,1433;Database=turbodbc;Encrypt=yes;TrustServerCertificate=no;UID=sa;PWD=QuantCo123"
    conn_string_components = {
        x.split("=")[0].lower(): x.split("=")[1] for x in odbc_string.split(";")
    }
    return turbodbc.connect(
        **conn_string_components,
        turbodbc_options=turbodbc.make_options(
            prefer_unicode=True, large_decimals_as_64_bit_types=True
        ),
    )


connection = mssql_connect_turbodbc()
start = perf_counter()
turbodbc_read_sql("SELECT * FROM turbodbc.dbo.single_big", connection)
print("Elapsed seconds:", perf_counter() - start)
