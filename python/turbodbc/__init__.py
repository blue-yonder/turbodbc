from importlib.metadata import version

from turbodbc_intern import Megabytes, Rows

from .api_constants import apilevel, paramstyle, threadsafety
from .connect import connect
from .constructors import Date, Time, Timestamp
from .data_types import BINARY, DATETIME, NUMBER, ROWID, STRING
from .exceptions import DatabaseError, Error, InterfaceError, ParameterError
from .options import make_options

try:
    __version__ = version(__name__)
except:  # noqa: E722
    __version__ = "unknown"

__all__ = [
    "Megabytes",
    "Rows",
    "apilevel",
    "paramstyle",
    "threadsafety",
    "connect",
    "Date",
    "Time",
    "Timestamp",
    "BINARY",
    "DATETIME",
    "NUMBER",
    "ROWID",
    "STRING",
    "DatabaseError",
    "Error",
    "InterfaceError",
    "ParameterError",
    "make_options",
]
