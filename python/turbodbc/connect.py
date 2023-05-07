from typing import Optional

from turbodbc_intern import connect as intern_connect

from .connection import Connection
from .exceptions import ParameterError, translate_exceptions
from .options import make_options


def _make_connection_string(dsn: Optional[str], **kwargs) -> str:
    if dsn:
        kwargs["dsn"] = dsn
    return ";".join([f"{key}={value}" for key, value in kwargs.items()])


@translate_exceptions
def connect(
    dsn: Optional[str] = None,
    turbodbc_options=None,
    connection_string: Optional[str] = None,
    **kwargs,
) -> Connection:
    r"""
    Create a connection with the database identified by the ``dsn`` or the ``connection_string``.

    :param dsn: Data source name as given in the (unix) odbc.ini file
           or (Windows) ODBC Data Source Administrator tool.
    :param turbodbc_options: Options that control how turbodbc interacts with the database.
           Create such a struct with `turbodbc.make_options()` or leave this blank to take the defaults.
    :param connection_string: Preformatted ODBC connection string.
           Specifying this and dsn or kwargs at the same time raises ParameterError.
    :param \**kwargs: You may specify additional options as you please. These options will go into
           the connection string that identifies the database. Valid options depend on the specific database you
           would like to connect with (e.g. `user` and `password`, or `uid` and `pwd`)
    :return: A connection to your database
    """
    if turbodbc_options is None:
        turbodbc_options = make_options()

    if connection_string is not None and (dsn is not None or len(kwargs) > 0):
        raise ParameterError("Both connection_string and dsn or kwargs specified")

    if connection_string is None:
        connection_string = _make_connection_string(dsn, **kwargs)

    connection = Connection(intern_connect(connection_string, turbodbc_options))

    return connection
