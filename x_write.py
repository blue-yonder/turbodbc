import sqlalchemy as sa

print("Hey")

ROWS = 1_000_000
AT_ONCE = 1

eng = sa.create_engine("mssql+pyodbc://sa:QuantCo123@localhost:1433/turbodbc?driver=libtdsodbc.so")

with eng.begin() as transaction:
    transaction.execute(sa.text("DELETE FROM single_big"))
    last = None
    for idx in range(0, ROWS, AT_ONCE):
        percentage = int(idx/ROWS * 100)
        if percentage % 10 == 0 and percentage != last:
            last = percentage
            print(percentage)

        value = "a" * 8_000
        param = ", ".join([f"('{value}') "] * AT_ONCE)
        transaction.execute(sa.text(f"INSERT INTO single_big VALUES {param}"))
