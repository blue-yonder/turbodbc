from collections import OrderedDict
from itertools import islice
from typing import Any, cast

import pyarrow as pa
from turbodbc_intern import (
    make_arrow_result_set,
    make_numpy_result_set,
    make_parameter_set,
    make_row_based_result_set,
    set_numpy_parameters,
)

from .exceptions import InterfaceError, translate_exceptions


def _make_masked_arrays(result_batch):
    from numpy import object_
    from numpy.ma import MaskedArray

    masked_arrays = []
    for data, mask in result_batch:
        if isinstance(data, list):
            masked_arrays.append(MaskedArray(data=data, mask=mask, dtype=object_))
        else:
            masked_arrays.append(MaskedArray(data=data, mask=mask))
    return masked_arrays


def _assert_numpy_column_preconditions(columns):
    from numpy import ndarray
    from numpy.ma import MaskedArray

    n_columns = len(columns)
    for index, column in enumerate(columns, start=1):
        if type(column) not in [MaskedArray, ndarray]:
            raise InterfaceError(
                "Bad type for column {} of {}. Only numpy.ndarray and numpy.ma.MaskedArrays are supported".format(
                    index, n_columns
                )
            )
        if column.ndim != 1:
            raise InterfaceError(
                f"Column {index} of {n_columns} is not one-dimensional"
            )
        if not column.flags.c_contiguous:
            raise InterfaceError(f"Column {index} of {n_columns} is not contiguous")

    lengths = [len(column) for column in columns]
    all_same_length = all(length == lengths[0] for length in lengths)
    if not all_same_length:
        raise InterfaceError(
            f"All columns must have the same length, got lengths {lengths}"
        )


class Cursor:
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

    def _assert_valid(self):
        if self.impl is None:
            raise InterfaceError("Cursor already closed")

    def _assert_valid_result_set(self):
        if self.result_set is None:
            raise InterfaceError("No active result set")

    @property
    def description(self):
        """
        Retrieve a description of the columns in the current result set

        :return: A tuple of seven elements. Only some elements are meaningful:\n
                 *   Element #0 is the name of the column
                 *   Element #1 is the type code of the column
                 *   Element #6 is true if the column may contain ``NULL`` values
        """
        if self.result_set:
            info = self.result_set.get_column_info()
            return [
                (c.name, c.type_code(), None, None, None, None, c.supports_null_values)
                for c in info
            ]
        else:
            return None

    def _execute(self):
        self.impl.execute()
        self.rowcount = self.impl.get_row_count()
        cpp_result_set = self.impl.get_result_set()
        if cpp_result_set:
            self.result_set = make_row_based_result_set(cpp_result_set)
        else:
            self.result_set = None
        return self

    @translate_exceptions
    def execute(self, sql, parameters=None):
        """
        Execute an SQL command or query

        :param sql: A (unicode) string that contains the SQL command or query. If you would like to
               use parameters, please use a question mark ``?`` at the location where the
               parameter shall be inserted.
        :param parameters: An iterable of parameter values. The number of values must match
               the number of parameters in the SQL string.
        :return: The ``Cursor`` object to allow chaining of operations.
        """
        self.rowcount = -1
        self._assert_valid()
        self.impl.prepare(sql)
        if parameters:
            buffer = make_parameter_set(self.impl)
            buffer.add_set(parameters)
            buffer.flush()
        return self._execute()

    @translate_exceptions
    def executemany(self, sql, parameters=None):
        """
        Execute an SQL command or query with multiple parameter sets passed in a row-wise fashion.
        This function is part of PEP-249.

        :param sql: A (unicode) string that contains the SQL command or query. If you would like to
               use parameters, please use a question mark ``?`` at the location where the
               parameter shall be inserted.
        :param parameters: An iterable of iterable of parameter values. The outer iterable represents
               separate parameter sets. The inner iterable contains parameter values for a given
               parameter set. The number of values of each set must match the number of parameters
               in the SQL string.
        :return: The ``Cursor`` object to allow chaining of operations.
        """
        self.rowcount = -1
        self._assert_valid()
        self.impl.prepare(sql)

        if parameters:
            buffer = make_parameter_set(self.impl)
            for parameter_set in parameters:
                buffer.add_set(parameter_set)
            buffer.flush()

        return self._execute()

    @translate_exceptions
    def executemanycolumns(self, sql: str, columns):
        """
        Execute an SQL command or query with multiple parameter sets that are passed in
        a column-wise fashion as opposed to the row-wise parameters in ``executemany()``.
        This function is a turbodbc-specific extension to PEP-249.

        :param sql: A (unicode) string that contains the SQL command or query. If you would like to
               use parameters, please use a question mark ``?`` at the location where the
               parameter shall be inserted.
        :param columns: An iterable of NumPy MaskedArrays. The Arrays represent the columnar
               parameter data,
        :return: The ``Cursor`` object to allow chaining of operations.
        """
        self.rowcount = -1
        self._assert_valid()

        self.impl.prepare(sql)

        def _num_chunks(c):
            return c.num_chunks

        if isinstance(columns, pa.Table):
            from turbodbc.arrow_support import set_arrow_parameters  # type: ignore

            for column in columns.itercolumns():
                if _num_chunks(column) != 1:
                    raise NotImplementedError("Chunked Arrays are " "not yet supported")

            set_arrow_parameters(self.impl, columns)
            return self._execute()

        _assert_numpy_column_preconditions(columns)

        from numpy.ma import MaskedArray

        split_arrays = []
        for column in columns:
            if isinstance(column, MaskedArray):
                split_arrays.append((column.data, column.mask, str(column.dtype)))
            else:
                split_arrays.append((column, False, str(column.dtype)))
        set_numpy_parameters(self.impl, split_arrays)

        return self._execute()

    @translate_exceptions
    def fetchone(self):
        """
        Returns a single row of a result set. Requires an active result set on the database
        generated with ``execute()`` or ``executemany()``.

        :return: Returns ``None`` when no more rows are available in the result set
        """
        self._assert_valid_result_set()
        result = cast(Any, self.result_set).fetch_row()
        if len(result) == 0:
            return None
        else:
            return result

    @translate_exceptions
    def fetchall(self):
        """
        Fetches a list of all rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :return: A list of rows
        """
        return [row for row in self]

    @translate_exceptions
    def fetchmany(self, size=None):
        """
        Fetches a batch of rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :param size: Controls how many rows are returned. The default ``None`` means that
               the value of Cursor.arraysize is used.
        :return: A list of rows
        """
        if size is None:
            size = self.arraysize
        if size <= 0:
            raise InterfaceError(f"Invalid arraysize {size} for fetchmany()")

        return [row for row in islice(self, size)]

    def fetchallnumpy(self):
        """
        Fetches all rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :return: An ``OrderedDict`` of *columns*, where the keys of the dictionary
                 are the column names. The columns are of NumPy's ``MaskedArray``
                 type, where the optimal data type for each result set column is
                 chosen automatically.
        """
        from numpy.ma import concatenate

        batches = list(self._numpy_batch_generator())
        column_names = [description[0] for description in self.description]
        return OrderedDict(
            zip(column_names, [concatenate(column) for column in zip(*batches)])
        )

    def fetchnumpybatches(self):
        """
        Returns an iterator over all rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :return: An iterator you can use to iterate over batches of rows of the result set. Each
                 batch consists of an ``OrderedDict`` of NumPy ``MaskedArray`` instances. See
                 ``fetchallnumpy()`` for details.
        """
        batchgen = self._numpy_batch_generator()
        column_names = [description[0] for description in self.description]
        for next_batch in batchgen:
            yield OrderedDict(zip(column_names, next_batch))

    def _numpy_batch_generator(self):
        self._assert_valid_result_set()

        numpy_result_set = make_numpy_result_set(self.impl.get_result_set())
        first_run = True
        while True:
            result_batch = _make_masked_arrays(numpy_result_set.fetch_next_batch())
            is_empty_batch = len(result_batch[0]) == 0
            if is_empty_batch and not first_run:
                return  # Let us return a typed result set at least once
            first_run = False
            yield result_batch

    def fetcharrowbatches(self, strings_as_dictionary=False, adaptive_integers=False):
        """
        Fetches rows in the active result set generated with ``execute()`` or
        ``executemany()`` as an iterable of arrow tables.

        :param strings_as_dictionary: If true, fetch string columns as
                 dictionary[string] instead of a plain string column.

        :param adaptive_integers: If true, instead of the integer type returned
                by the database (driver), this produce integer columns with the
                smallest possible integer type in which all values can be
                stored. Be aware that here the type depends on the resulting
                data.

        :return: generator of ``pyarrow.Table``
        """
        self._assert_valid_result_set()

        rs = make_arrow_result_set(
            self.impl.get_result_set(), strings_as_dictionary, adaptive_integers
        )
        first_run = True
        while True:
            table = rs.fetch_next_batch()
            is_empty_batch = len(table) == 0
            if is_empty_batch and not first_run:
                return  # Let us return a typed result set at least once
            first_run = False
            yield table

    def fetchallarrow(self, strings_as_dictionary=False, adaptive_integers=False):
        """
        Fetches all rows in the active result set generated with ``execute()`` or
        ``executemany()``.

        :param strings_as_dictionary: If true, fetch string columns as
                 dictionary[string] instead of a plain string column.

        :param adaptive_integers: If true, instead of the integer type returned
                by the database (driver), this produce integer columns with the
                smallest possible integer type in which all values can be
                stored. Be aware that here the type depends on the resulting
                data.

        :return: ``pyarrow.Table``
        """
        self._assert_valid_result_set()

        return make_arrow_result_set(
            self.impl.get_result_set(), strings_as_dictionary, adaptive_integers
        ).fetch_all()

    def nextset(self) -> bool:
        """
        makes cursor skip to the next available result set, discarding any remaining rows from the
        current result set. If there are no more result sets, the method returns False.
        Otherwise, it returns a True and subsequent calls to the fetch methods will
        return rows from the next result set.
        """
        self._assert_valid_result_set()
        return self.impl.more_results()

    def close(self):
        """
        Close the cursor.
        """
        self.result_set = None
        if self.impl is not None:
            self.impl._reset()
        self.impl = None

    def setinputsizes(self, sizes):
        """
        Has no effect since turbodbc automatically picks appropriate
        return types and sizes. Method exists since PEP-249 requires it.
        """
        pass

    def setoutputsize(self, size, column=None):
        """
        Has no effect since turbodbc automatically picks appropriate
        input types and sizes. Method exists since PEP-249 requires it.
        """
        pass

    def __enter__(self):
        """
        Conformance to PEP-343
        """
        return self

    def __exit__(self, type, value, traceback):
        """
        Conformance to PEP-343
        """
        return self.close()
