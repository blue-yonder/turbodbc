import datetime

from turbodbc import Date, Time, Timestamp


def test_constructors_date():
    d = Date(2016, 1, 4)
    assert d == datetime.date(2016, 1, 4)


def test_constructors_time():
    t = Time(1, 2, 3)
    assert t == datetime.time(1, 2, 3)


def test_constructors_timestamp():
    ts = Timestamp(2016, 1, 2, 3, 4, 5)
    assert ts == datetime.datetime(2016, 1, 2, 3, 4, 5)
