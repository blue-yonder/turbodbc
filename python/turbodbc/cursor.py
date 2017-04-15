from __future__ import absolute_import

from itertools import islice
from collections import OrderedDict

from turbodbc_intern import make_row_based_result_set, make_parameter_set

from .exceptions import translate_exceptions, InterfaceError, Error

def _has_numpy_support():

    try:
        import turbodbc_numpy_support
        return True
    except ImportError:
        return False

def _make_masked_arrays(result_batch):
    from numpy.ma import MaskedArray
    from numpy import object_
    masked_arrays = []
    for data, mask in result_batch:
        if isinstance(data, list):
            masked_arrays.append(MaskedArray(data=data, mask=mask, dtype=object_))
        else:
            masked_arrays.append(MaskedArray(data=data, mask=mask))
    return masked_arrays

class Cursor(object):
    """
    This class allows you to send SQL commands and queries to a database and retrieve
    associated result sets.
    """
    def __init__(self, impl):
        self.impl = impl
        self.result_set = None
        self.rowcount = -1
        self.arraysize = 1

    def __iter__(self):
        return self

    def __next__(self):
        element = self.fetchone()
        if element is None:
            raise StopIteration
        else:
            return element

    next = __next__  # Python 2 compatibility

    def _assert_valid(self):
        if self.impl is None:
            raise InterfaceError("Cursor already closed")

    def _assert_valid_result_set(self):
        if self.result_set is None:
            raise InterfaceError("No active result set")

    @property
    def description(self):
        """
        Retrieve a description of the columns in the current result set.

        :return: A tuple of seven elements (or just ``None`` if no result set exists). Only some elements are meaningful:\n
                 *   Element #0 is the name of the column
                 *   Element #1 is the type code of the column
                 *   Element #6 is true if the column may contain `NULL` values
        """
        if self.result_set:
            info = self.result_set.get_column_info()
            return [(c.name, c.type_code(), None, None, None, None, c.supports_null_values) for c in info]
        else:
            return None

    @translate_exceptions
    def execute(self, sql, parameters=None):
        """
        Execute an SQL command or query.

        :param sql: A (unicode) string that contains the SQL command or query. If you would
         like to use parameters, use a question mark ``?`` at the location where each
         parameter value should be substituted.
        :param parameters: An iterable of parameter values. The parameter values are matched
         by position with the question marks in the SQL string and the number of parameter
         values must match the number of question marks.
        :return: The ``Cursor`` object, to allow chaining of operations.
        """
        self.rowcount = -1
        self._assert_valid()
        self.impl.prepare(sql)
        if parameters:
            buffer = make_parameter_set(self.impl)
            buffer.add_set(parameters)
            buffer.flush()
        self.impl.execute()
        self.rowcount = self.impl.get_row_count()
        cpp_result_set = self.impl.get_result_set()
        if cpp_result_set:
            self.result_set = make_row_based_result_set(cpp_result_set)
        else:
            self.result_set = None
        return self

    @translate_exceptions
    def executemany(self, sql, parameters=None):
        """
        Execute an SQL command or query with multiple parameter sets.

        :param sql: A (unicode) string that contains the SQL command or query. If you would
         like to use parameters, use a question mark ``?`` at the location where each
         parameter value should be substituted.
        :param parameters: An iterable of iterables of parameter values. The outer iterable represents
         separate parameter sets. Each inner iterable contains parameter values for a given
         parameter set. The number of values of each set must match the number of question marks
         in the SQL string.
        :return: The ``Cursor`` object, to allow chaining of operations.
        """
        self.rowcount = -1
        self._assert_valid()
        self.impl.prepare(sql)

        if parameters:
            buffer = make_parameter_set(self.impl)
            for parameter_set in parameters:
                buffer.add_set(parameter_set)
            buffer.flush()

        self.impl.execute()
        self.rowcount = self.impl.get_row_count()
        cpp_result_set = self.impl.get_result_set()
        if cpp_result_set:
            self.result_set = make_row_based_result_set(cpp_result_set)
        else:
            self.result_set = None
        return self

    @translate_exceptions
    def fetchone(self):
        """
        Retrieve the next row of a result set. Requires an active result set on the database
        generated with ``execute()`` or ``executemany()``.

        :return: A single row, or ``None`` if no more rows are available
        """
        self._assert_valid_result_set()
        result = self.result_set.fetch_row()
        if len(result) == 0:
            return None
        else:
            return result

    @translate_exceptions
    def fetchall(self):
        """
        Retrieve all remaining rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :return: A list of rows, or an empty list if no more rows are available
        """
        return [row for row in self]

    @translate_exceptions
    def fetchmany(self, size=None):
        """
        Retrieve multiple rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :param size: Controls how many rows are returned. The default ``None`` means that
         the value of Cursor.arraysize is used.
        :return: A list of rows
        """
        if size is None:
            size = self.arraysize
        if (size <= 0):
            raise InterfaceError("Invalid arraysize {} for fetchmany()".format(size))

        return [row for row in islice(self, size)]

    def fetchallnumpy(self):
        """
        Retrieve all rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :return: An ``OrderedDict`` of *columns*, where the keys of the dictionary
                 are the column names. The columns are of NumPy's ``MaskedArray``
                 type, where the optimal data type for each result set column is
                 chosen automatically.
        """
        from numpy.ma import concatenate
        batches = list(self._numpy_batch_generator())
        column_names = [description[0] for description in self.description]
        return OrderedDict(zip(column_names, [concatenate(column) for column in zip(*batches)]))

    def fetchnumpybatches(self):
        """
        Return an iterator for all rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :return: An iterator you can use to iterate over batches of rows in the result set. Each
                 batch consists of an ``OrderedDict`` of NumPy ``MaskedArray`` instances. See
                 ``fetchallnumpy()`` for details.
        """
        batchgen = self._numpy_batch_generator()
        column_names = [description[0] for description in self.description]
        for next_batch in batchgen:
            yield OrderedDict(zip(column_names, next_batch))

    def _numpy_batch_generator(self):
        self._assert_valid_result_set()
        if _has_numpy_support():
            from turbodbc_numpy_support import make_numpy_result_set
        else:
            raise Error("turbodbc was compiled without numpy support. Please install "
                        "numpy and reinstall turbodbc")
        numpy_result_set = make_numpy_result_set(self.impl.get_result_set())
        first_run = True
        while True:
            result_batch = _make_masked_arrays(numpy_result_set.fetch_next_batch())
            is_empty_batch = (len(result_batch[0]) == 0)
            if is_empty_batch and not first_run:
                raise StopIteration # Let us return a typed result set at least once
            first_run = False
            yield result_batch

    def close(self):
        """
        Close the cursor. The cursor can no longer be used and any remaining result set on the cursor is discarded.
        """
        self.result_set = None
        self.impl = None

    def setinputsizes(self, sizes):
        """
        Has no effect since turbodbc automatically picks appropriate
        return types and sizes. Method is here only because PEP-249 requires it.
        """
        pass

    def setoutputsize(self, size, column=None):
        """
        Has no effect since turbodbc automatically picks appropriate
        input types and sizes. Method is here only because PEP-249 requires it.
        """
        pass
