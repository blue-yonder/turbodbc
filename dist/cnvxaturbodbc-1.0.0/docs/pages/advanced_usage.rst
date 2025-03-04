.. _advanced_usage:

Advanced usage
==============

.. _advanced_usage_options:

Performance, compatibility, and behavior options
------------------------------------------------

Turbodbc offers a way to adjust its behavior to tune performance and to
achieve compatibility with your database. The basic usage is this:

::

    >>> from turbodbc import connect, make_options
    >>> options = make_options()
    >>> connect(dsn="my_dsn", turbodbc_options=options)

This will connect with your database using the default options. To use non-default
options, supply keyword arguments to ``make_options()``:

::

    >>> from turbodbc import Megabytes
    >>> options = make_options(read_buffer_size=Megabytes(100),
    ...                        parameter_sets_to_buffer=1000,
    ...                        varchar_max_character_limit=10000,
    ...                        use_async_io=True,
    ...                        prefer_unicode=True,
    ...                        autocommit=True,
    ...                        large_decimals_as_64_bit_types=True,
    ...                        limit_varchar_results_to_max=True)


.. _advanced_usage_options_read_buffer:

Read buffer size
~~~~~~~~~~~~~~~~

``read_buffer_size`` affects how many result set rows are retrieved per batch
of results. Set the attribute to ``turbodbc.Megabytes(42)`` to have turbodbc determine
the optimal number of rows per batch so that the total buffer amounts to
42 MB. This is recommended for most users and databases. You can also set
the attribute to ``turbodbc.Rows(13)`` if you would like to fetch results in
batches of 13 rows. By default, turbodbc fetches results in batches of 20 MB.

Please note that sometimes a single row of a result set may exceed the specified
buffer size. This can happen if large fields such as ``VARCHAR(8000000)`` or ``TEXT``
are part of the result set. In this case, results are fetched in batches of single rows
that exceed the specified size. Buffer sizes for large text fields can be controlled
with the :ref:`advanced_usage_options_varchar_max` and XXX options.

.. _advanced_usage_options_write_buffer:

Buffered parameter sets
~~~~~~~~~~~~~~~~~~~~~~~

Similarly, ``parameter_sets_to_buffer`` changes the number of parameter sets
which are transferred per batch of parameters (e.g., as sent with ``executemany()``).
Please note that it is not (yet) possible to use the `Megabytes` and `Rows` classes
here.


.. _advanced_usage_options_varchar_max:

VARCHAR(max) character limit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``varchar_max_character_limit`` specifies the buffer size for result set columns
of types ``VARCHAR(max)``, ``NVARCHAR(max)``, or similar types your database supports.
Small values increase the chance of truncation, large ones require more memory. Depending
on your setting of ``read_buffer_size``, this may increase the total memory consumption
or reduce the number of rows fetched per batch, thus affecting performance.
The default value is ``65535`` characters.

.. note::
    This value does not affect fields of type ``VARCHAR(n)`` with ``n > 0``, unless
    the option :ref:`advanced_usage_options_limit_varchar_results` is set. Also, this
    option does not affect parameters that you may pass to the database.


Asynchronous input/output
~~~~~~~~~~~~~~~~~~~~~~~~~

If you set ``use_async_io`` to ``True``, turbodbc will use asynchronous I/O operations
(limited to result sets for the time being). Asynchronous I/O means that while the
main thread converts result set rows retrieved from the database to Python
objects, another thread fetches a new batch of results from the database in the background. This may yield
a speedup of ``2`` if retrieving and converting are similarly fast
operations.

.. note::
    Asynchronous I/O is experimental and has to fully prove itself yet.
    Do not be afraid to give it a try, though.


.. _advanced_usage_options_prefer_unicode:

Prefer unicode
~~~~~~~~~~~~~~

Set ``prefer_unicode`` to ``True`` if your database does not fully support
the UTF-8 encoding turbodbc prefers. With this option you can tell turbodbc
to use two-byte character strings with UCS-2/UTF-16 encoding. Use this option
if you wish to connect to Microsoft SQL Server (MSSQL).


.. _advanced_usage_options_autocommit:

Autocommit
~~~~~~~~~~

Set ``autocommit`` to ``True`` if you want the database to ``COMMIT`` your
changes automatically after each query or command. By default, ``autocommit``
is disabled and users are required to call ``connection.commit()`` to persist
their changes.

.. note::
    Some databases that do not support transactions may even require this
    option to be set to ``True`` in order to establish a connection at all.


.. _advanced_usage_options_large_decimals:

Large decimals as 64 bit types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Set ``large_decimals_as_64_bit_types`` to ``True`` if you want to retrieve
``Decimal`` and ``Numeric`` types with more than ``18`` digits as 64 bit
``integer`` and ``float`` numbers. The default is to retrieve such fields
as strings instead.

Please note that this option may lead to overflows or loss of precision. If,
however, your data type is much larger than the data it is supposed to hold,
this option is very useful to obtain numeric Python objects and
:ref:`NumPy arrays <advanced_usage_numpy>`.


.. _advanced_usage_options_limit_varchar_results:

Limit VARCHAR results to MAX
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Set ``limit_varchar_results_to_max`` to ``True`` if you want to limit *all*
string-like fields (``VARCHAR(n)``, ``NVARCHAR(n)``, etc. with ``n > 0``) in
result sets to a maximum of :ref:`advanced_usage_options_varchar_max` characters.

Please note that enabling this option can lead to truncation of string-like
data when retrieving results. Parameters sent to the database are not
affected by this option.

If not set or set to ``False``, string-like result fields with a specific size will
*always* be retrieved with a sufficiently large buffer so that no truncation occurs.
String-like fields of indeterminate size (``VARCHAR(max)``, ``TEXT``, etc. on some
databases) are still subject to :ref:`advanced_usage_options_varchar_max`.

.. _advanced_usage_options_unicode_extra_capacity:

Extra capacity for unicode strings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Set ``force_extra_capacity_for_unicode`` to ``True`` if  you find that strings retrieved
from ``VARCHAR(n)`` or ``NVARCHAR(n)`` fields are being truncated. Some ODBC drivers report
the length of the field and setting this option changes the way turbodbc allocates memory,
so that retrieving these strings are not truncated. If ``limit_varchar_results_to_max`` is
``True``, memory is allocated as if ``n`` is :ref:`advanced_usage_options_varchar_max`.

Please note that enabling this option leads to increased memory usage when retrieving string
fields in result sets. Parameters sent to the database are not affected by this option.

.. _advanced_usage_options_wide_chars_as_narrow:

Decoding wide character types as narrow types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Set ``fetch_wchar_as_char`` to ``True`` if  you find that strings retrieved
from ``NVARCHAR(n)`` fields are being corrupted. Some ODBC drivers place single byte encodings
into ``SQL_WCHAR`` type strings and as a consequence are corrupted upon retrieval by turbodbc.
Setting this option forces turbodbc to decode ``SQL_WCHAR`` as single byte encodings.


Controlling autocommit behavior at runtime
------------------------------------------

You can enable and disable autocommit mode after you have established a connection,
and you can also check whether autocommit is currently enabled:

::

    >>> from turbodbc import connect
    >>> connection = connect(dsn="my DSN")
    >>> connection.autocommit = True

    [... more things happening ...]

    >>> if not connection.autocommit:
    ...     connection.commit()


.. _advanced_usage_numpy:

NumPy support
-------------

.. note::
    Turbodbc's NumPy support requires the ``numpy`` package to be installed. For all source builds,
    Numpy needs to be installed before installing turbodbc.
    Please check the :ref:`installation instructions <getting_started_installation>`
    for more details.


Obtaining NumPy result sets all at once
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here is how to use turbodbc to retrieve the full result set in the form of NumPy
masked arrays:

::

    >>> cursor.execute("SELECT A, B FROM my_table")
    >>> cursor.fetchallnumpy()
    OrderedDict([('A', masked_array(data = [42 --],
                                    mask = [False True],
                                    fill_value = 999999)),
                 ('B', masked_array(data = [3.14 2.71],
                                    mask = [False False],
                                    fill_value = 1e+20))])


Obtaining NumPy result sets in batches
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can also fetch NumPy result sets in batches using an iterable:

::

    >>> cursor.execute("SELECT A, B FROM my_table")
    >>> batches = cursor.fetchnumpybatches()
    >>> for batch in batches:
    ...     print(batch)
    OrderedDict([('A', masked_array(data = [42 --],
                                    mask = [False True],
                                    fill_value = 999999)),
                 ('B', masked_array(data = [3.14 2.71],
                                    mask = [False False],
                                    fill_value = 1e+20))])

The size of the batches depends on the ``read_buffer_size`` attribute set in
the :ref:`performance options <advanced_usage_options_read_buffer>`.


Notes regarding NumPy result sets
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


*   NumPy results are returned as an ``OrderedDict`` of column name/value pairs. The column
    order is the same as in your query.
*   The column values are of type ``MaskedArray``. Any ``NULL`` values you have in your
    database will show up as masked entries (``NULL`` values in string-like columns
    will show up as ``None`` objects).

The following table shows how the most common data types data scientists are interested in
are converted to NumPy columns:

+-----------------------------------+------------------------------+
| Database type(s)                  | Python type                  |
+===================================+==============================+
| Integers, ``DECIMAL(<19,0)``      | ``int64``                    |
+-----------------------------------+------------------------------+
| ``DOUBLE``, ``DECIMAL(<19, >0)``  | ``float64``                  |
+-----------------------------------+------------------------------+
| ``DECIMAL(>18, 0)``               | ``object_`` or ``int64`` *   |
+-----------------------------------+------------------------------+
| ``DECIMAL(>18, >0)``              | ``object_`` or ``float64`` * |
+-----------------------------------+------------------------------+
| ``BIT``, boolean-like             | ``bool_``                    |
+-----------------------------------+------------------------------+
| ``TIMESTAMP``, ``TIME``           | ``datetime64[µs]``           |
+-----------------------------------+------------------------------+
| ``DATE``                          | ``datetime64[D]``            |
+-----------------------------------+------------------------------+
| ``VARCHAR``, strings              | ``object_``                  |
+-----------------------------------+------------------------------+

\*) The conversion depends on turbodbc's ``large_decimals_as_64_bit_types``
:ref:`option <advanced_usage_options_large_decimals>`.


.. _advanced_usage_numpy_parameters:

Using NumPy arrays as query parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here is how to use turbodbc to use values stored in NumPy arrays
as query parameters with ``executemanycolumns()``:

::

    >>> from numpy import array
    >>> from numpy.ma import MaskedArray
    >>> normal_param = array([1, 2, 3], dtype='int64')
    >>> masked_param = MaskedArray([3.14, 1.23, 4.56],
    ...                            mask=[False, True, False],
    ...                            dtype='float64')

    >>> cursor.executemanycolumns("INSERT INTO my_table VALUES (?, ?)",
    ...                           [normal_param, masked_param])
    # functionally equivalent, but much faster than:
    # cursor.execute("INSERT INTO my_table VALUES (1, 3.14)")
    # cursor.execute("INSERT INTO my_table VALUES (2, NULL)")
    # cursor.execute("INSERT INTO my_table VALUES (3, 4.56)")

    >>> cursor.execute("SELECT * FROM my_table").fetchall()
    [[1L, 3.14], [2L, None], [3L, 4.56]]

*   Columns must either be of type ``MaskedArray`` or ``ndarray``.
*   Each column must contain one-dimensional, contiguous data.
*   All columns must have equal size.
*   The ``dtype`` of each column must be supported, see the table below.
*   Use ``MaskedArray`` and set the ``mask`` to ``True`` for individual
    elements to use ``None`` values.
*   Data is transferred in batches (see :ref:`advanced_usage_options_write_buffer`)


+-------------------------------------------------------------------------+--------------------------------+
| Supported NumPy type                                                    | Transferred as                 |
+=========================================================================+================================+
| ``int64``                                                               | ``BIGINT`` (64 bits)           |
+-------------------------------------------------------------------------+--------------------------------+
| ``float64``                                                             | ``DOUBLE PRECISION`` (64 bits) |
+-------------------------------------------------------------------------+--------------------------------+
| ``bool_``                                                               | ``BIT``                        |
+-------------------------------------------------------------------------+--------------------------------+
| ``datetime64[µs]``                                                      | ``TIMESTAMP``                  |
+-------------------------------------------------------------------------+--------------------------------+
| ``datetime64[ns]``                                                      | ``TIMESTAMP``                  |
+-------------------------------------------------------------------------+--------------------------------+
| ``datetime64[D]``                                                       | ``DATE``                       |
+-------------------------------------------------------------------------+--------------------------------+
| ``object_`` (only ``str``, ``unicode``, and ``None`` objects supported) | ``VARCHAR`` (automatic sizing) |
+-------------------------------------------------------------------------+--------------------------------+

.. _advanced_usage_arrow:

Apache Arrow support
--------------------

.. note::
    Turbodbc's Apache Arrow support requires the ``pyarrow`` package to be installed.
    For all source builds, Apache Arrow needs to be installed before installing turbodbc.
    Please check the :ref:`installation instructions <getting_started_installation>`
    for more details.

`Apache Arrow <https://arrow.apache.org>`_ is a high-performance data layer that
is built for cross-system columnar in-memory analytics using a
`data model <https://arrow.apache.org/docs/python/data.html>`_ designed to make the
most of the CPU cache and vector operations.

.. note::
    Apache Arrow support in turbodbc is still experimental and may not be as efficient
    as possible yet. Also, Apache Arrow support is not yet available for Windows and
    has some issues with Unicode fields. Stay tuned for upcoming improvements.

Obtaining Apache Arrow result sets
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here is how to use turbodbc to retrieve the full result set in the form of an
Apache Arrow table:

::

    >>> cursor.execute("SELECT A, B FROM my_table")
    >>> table = cursor.fetchallarrow()
    >>> table
    pyarrow.Table
    A: int64
    B: string
    >>> table[0].to_pylist()
    [42]
    >>> table[1].to_pylist()
    [u'hello']

Looking at the data like this is not particularly useful. However, there is some
really useful stuff you can do with an Apache Arrow table, for example,
`convert it to a Pandas dataframe <https://arrow.apache.org/docs/python/pandas.html>`_
like this:

::

    >>> table.to_pandas()
        A      B
    0  42  hello


As a performance optimisation for string columns, you can specify the parameter
``strings_as_dictionary``. This will retrieve all string columns as Arrow
``DictionaryArray``. The data will here be split into two arrays, one that stores
all unique string values and one integer array that stores for each row the index
in the dictionary. On conversion to Pandas, these columns will be turned into
``pandas.Categorical``.

::

    >>> cursor.execute("SELECT a, b FROM my_other_table")
    >>> table = cursor.fetchallarrow(strings_as_dictionary=True)
    >>> table
    pyarrow.Table
    a: int64
    b: dictionary<values=binary, indices=int8, ordered=0>
      dictionary: [61, 62]
    >>> table.to_pandas()
       a  b
    0  1  a
    1  2  b
    2  3  b
    >>> table.to_pandas().info()
    <class 'pandas.core.frame.DataFrame'>
    Int64Index: 3 entries, 0 to 2
    Data columns (total 2 columns):
    a    3 non-null int64
    b    3 non-null category
    dtypes: category(1), int64(1)
    memory usage: 147.0 bytes


To further reduce the memory usage of the returned results, the Arrow based
interface can return the integer columns as the minimal possible integer
storage type. This type can be different from the integer type used and
returned by the database. This mode can be activated by setting
``adaptive_integers=True``.

::

    >>> # Standard result retrieval
    >>> cursor.execute("SELECT * FROM (VALUES(1), (2), (3))")
    >>> table = cursor.fetchallarrow()
    >>> table
    pyarrow.Table
    __COL0__: int64
    >>> table.to_pandas()
       __COL0__
    0         1
    1         3
    2         2
    >>> table.to_pandas().info()
    <class 'pandas.core.frame.DataFrame'>
    RangeIndex: 3 entries, 0 to 2
    Data columns (total 1 columns):
    __COL0__    3 non-null int64
    dtypes: int64(1)
    memory usage: 96.0 bytes

    >>> # With adaptive integer storage
    >>> cursor.execute("SELECT * FROM (VALUES(1), (2), (3))")
    >>> table = cursor.fetchallarrow(adaptive_integers=True)
    >>> table
    pyarrow.Table
    __COL0__: int8
    >>> table.to_pandas()
       __COL0__
    0         1
    1         3
    2         2
    >>> table.to_pandas().info()
    <class 'pandas.core.frame.DataFrame'>
    RangeIndex: 3 entries, 0 to 2
    Data columns (total 1 columns):
    __COL0__    3 non-null int8
    dtypes: int8(1)
    memory usage: 75.0 bytes


Obtaining Apache Arrow result sets in batches
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similar to the numpy support for fetching results as a batch, you can
fetch a query result as an iterator of pyarrow tables.

::

    >>> cursor.execute("SELECT A, B FROM my_table")
    >>> batches = cursor.fetcharrowbatches()
    >>> for batch in batches:
    ...     print(batch)
    pyarrow.Table


The size of the batches depends on the ``read_buffer_size`` attribute set in
the :ref:`performance options <advanced_usage_options_read_buffer>`.


.. _advanced_usage_arrow_parameters:

Using Apache Arrow tables as query parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here is how to use turbodbc to use values stored in Apache Arrow
tables as query parameters with ``executemanycolumns()``:

::

    >>> import numpy as np
    >>> import pyarrow as pa
    >>> normal_param = pa.array([1, 2, 3], type=pa.int64())
    >>> masked_param = pa.Array.from_pandas(np.array([3.14, 1.23, 4.56])
    ...                            mask=np.array([False, True, False])
    ...                            type=pa.float64())
    >>> table = pa.Table.from_arrays([normal_param, masked_param], ['a', 'b'])

    >>> cursor.executemanycolumns("INSERT INTO my_table VALUES (?, ?)",
    ...                           table)
    # functionally equivalent, but much faster than:
    # cursor.execute("INSERT INTO my_table VALUES (1, 3.14)")
    # cursor.execute("INSERT INTO my_table VALUES (2, NULL)")
    # cursor.execute("INSERT INTO my_table VALUES (3, 4.56)")

    >>> cursor.execute("SELECT * FROM my_table").fetchall()
    [[1L, 3.14], [2L, None], [3L, 4.56]]

*   Tables must be of type ``pyarrow.Table``.
*   Each column must contain one-dimensional, contiguous data. There is
    no support for chunked arrays yet.
*   All columns must have equal size.
*   The ``dtype`` of each column must be supported, see the table below.
*   Data is transferred in batches (see :ref:`advanced_usage_options_write_buffer`)


+------------------------------------------------------------------------------+--------------------------------+
| Supported Apache Arrow type                                                  | Transferred as                 |
+==============================================================================+================================+
| ``INT8``, ``UINT8``, ``INT16``, ``UINT16``, ``INT32``, ``UINT32``, ``INT64`` | ``BIGINT`` (64 bits)           |
+------------------------------------------------------------------------------+--------------------------------+
| ``DOUBLE``                                                                   | ``DOUBLE PRECISION`` (64 bits) |
+------------------------------------------------------------------------------+--------------------------------+
| ``BOOL``                                                                     | ``BIT``                        |
+------------------------------------------------------------------------------+--------------------------------+
| ``TIMESTAMP[µs]``                                                            | ``TIMESTAMP``                  |
+------------------------------------------------------------------------------+--------------------------------+
| ``TIMESTAMP[ns]``                                                            | ``TIMESTAMP``                  |
+------------------------------------------------------------------------------+--------------------------------+
| ``DATE32``                                                                   | ``DATE``                       |
+------------------------------------------------------------------------------+--------------------------------+
| ``BINARY``                                                                   | ``VARCHAR`` (automatic sizing) |
+------------------------------------------------------------------------------+--------------------------------+
| ``STRING``                                                                   | ``VARCHAR`` (automatic sizing) |
+------------------------------------------------------------------------------+--------------------------------+

Using multiple results sets
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Some commands will return multi result sets, you can cycle through them with the
`nextset` function. It will discard the current result set and load up the next
result set, if another was loaded it return True. If no additional set is found
it returns False:
::

    >>> cursor.execute("SELECT 4;SELECT 2;")
    >>> first_set = cursor.fetchall()
    >>> while(cursor.nextset()):
    ...     remaining_set = cursor.fetchall();
    >>> assert first_set = [[4]]
    >>> assert remaining_set = [[2]]
::
