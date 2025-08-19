from functools import wraps

from turbodbc_intern import Error as InternError
from turbodbc_intern import InterfaceError as InternInterfaceError


class Error(Exception):
    """
    turbodbc's basic error class
    """

    pass


class InterfaceError(Error):
    """
    An error that is raised whenever you use turbodbc incorrectly
    """

    pass


class DatabaseError(Error):
    """
    An error that is raised when the database encounters an error while processing
    your commands and queries
    """

    pass


class ParameterError(Error):
    """
    An error that is raised when you use connection arguments that are supposed
    to be mutually exclusive
    """

    pass


def translate_exceptions(f):
    @wraps(f)
    def wrapper(*args, **kwds):
        try:
            return f(*args, **kwds)
        except InternError as e:
            raise DatabaseError(str(e))
        except InternInterfaceError as e:
            raise InterfaceError(str(e))

    return wrapper
