import sqlalchemy as sa

from pathlib import Path
from typing import List


print("Hey")

AT_ONCE = 1

eng = sa.create_engine(Path("./x_conn_str_sa.txt").read_text(encoding="utf-8").strip("\n"))


def writeit(table_name: str, row_values: List[str], types: List[str], nrows: str):
    with eng.begin() as transaction:
        transaction.execute(sa.text(f"DROP TABLE IF EXISTS {table_name}"))

        stmt_types = ",\n".join([f"v_{idx} {type_}" for idx, type_ in enumerate(types)])
        transaction.execute(sa.text(f"CREATE TABLE {table_name} ({stmt_types})"))

        last = None
        for idx in range(0, nrows, AT_ONCE):
            percentage = int(idx/nrows * 100)
            if percentage % 10 == 0 and percentage != last:
                last = percentage
                print(percentage)

            stmt_values = ",".join([f"'{value}'" for value in row_values])
            param = ", ".join([f"({stmt_values}) "] * AT_ONCE)
            transaction.execute(sa.text(f"INSERT INTO {table_name} VALUES {param}"))


which = input("Which input: ")

if which == "varchar":
    writeit("varchar_100k", ["a" * 8000], ["VARCHAR(8000)"], 100_000)
elif which == "varchar_80":
    writeit("varchar_100k_80", ["a" * 80], ["VARCHAR(8000)"], 100_000)
elif which == "char":
    writeit("char_1m", ["a" * 80], ["CHAR(80)"], 1_000_000)
elif which == "tinyint":
    writeit("tinyint_50m", [123], ["TINYINT"], 50_000_000)
elif which == "date":
    writeit("date_20m", ["2023-10-04"], ["DATE"], 20_000_000)
elif which == "all":
    writeit("all_100k", ["v" * 8000, "c" * 8000, 123, "2023-10-04"], ["VARCHAR(8000)", "CHAR(8000)", "TINYINT", "DATE"], 100_000)
elif which == "varchar_many":
    writeit("varchar_many_100k", ["a" * 80] * 100, ["VARCHAR(80)"] * 100, 100_000)
elif which == "tinyint_many":
    writeit("tinyint_many_100k", [123] * 100, ["TINYINT"] * 100, 100_000)
elif which == "date_many":
    writeit("date_many_100k", ["2023-10-04"] * 100, ["DATE"] * 100, 100_000)
elif which == "char_many":
    writeit("char_many_100k", ["a" * 80] * 100, ["CHAR(80)"] * 100, 100_000)
else:
    raise ValueError(which)
