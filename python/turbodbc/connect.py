from __future__ import absolute_import

import warnings
import six

from turbodbc_intern import connect as intern_connect

from .exceptions import translate_exceptions, ParameterError
from .connection import Connection
from .options import make_options

def _make_connection_string(dsn, **kwargs):
    if dsn:
        kwargs['dsn'] = dsn
    return ';'.join(["{}={}".format(key, value) for key, value in six.iteritems(kwargs)])


@translate_exceptions
def connect(dsn=None, turbodbc_options=None, read_buffer_size=None, parameter_sets_to_buffer=None, use_async_io=False, connection_string=None, **kwargs):
    """
    Create a connection with the database identified by the ``dsn`` or the ``connection_string``.

    :param dsn: Data source name as given in the (unix) odbc.ini file or (Windows) ODBC Data Source.
    :param turbodbc_options: Options that control how turbodbc interacts with the database.
     Create an options struct with `turbodbc.make_options()` or leave this blank to use the defaults.
    :param connection_string: Pre-formatted ODBC connection string.
     Specifying this and dsn or kwargs at the same time raises ParameterError.
    :param \**kwargs: You may specify additional options as you please. These options will go into
     the connection string that identifies the database. Valid options depend on the specific database you
     would like to connect to (e.g. `user` and `password`, or `uid` and `pwd`)
    :return: A connection to your database
    """
    if turbodbc_options is None:
        turbodbc_options = make_options(read_buffer_size=read_buffer_size,
                                        parameter_sets_to_buffer=parameter_sets_to_buffer,
                                        use_async_io=use_async_io,
                                        prefer_unicode=False)

    if read_buffer_size:
        warnings.warn("Calling turbodbc.connect() with parameter read_buffer_size is deprecated. "
                      "Please use make_options() instead.", DeprecationWarning)
    if parameter_sets_to_buffer:
        warnings.warn("Calling turbodbc.connect() with parameter parameter_sets_to_buffer is deprecated. "
                      "Please use make_options() instead.", DeprecationWarning)
    if use_async_io:
        warnings.warn("Calling turbodbc.connect() with parameter use_async_io is deprecated. "
                      "Please use make_options() instead.", DeprecationWarning)

    if connection_string is not None and (dsn is not None or len(kwargs) > 0):
        raise ParameterError("Both connection_string and dsn or kwargs specified")

    if connection_string is None:
        connection_string = _make_connection_string(dsn, **kwargs)

    connection = Connection(intern_connect(connection_string,
                                           turbodbc_options))

    return connection