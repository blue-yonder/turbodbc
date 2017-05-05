.. _getting_started:

Getting started
===============

.. _getting_started_installation:

Installation
------------

Linux and OSX
~~~~~~~~~~~~~


To install turbodbc on Linux and OSX, please use the following command:

::

    pip install turbodbc

This will trigger a source build that requires compiling C++ code. Please make sure
the following prerequisites are met:

+-----------------------------+-----------------------------+--------------------------+
| Requirement                 | Linux (``apt-get install``) | OSX (``brew install``)   |
+=============================+=============================+==========================+
| C++11 compiler              | G++-4.8 or higher           | clang with OSX 10.9+     |
+-----------------------------+-----------------------------+--------------------------+
| Boost library + headers (1) | ``libboost-all-dev``        | ``boost``                |
+-----------------------------+-----------------------------+--------------------------+
| ODBC library + headers      | ``unixodbc-dev``            | ``unixodbc``             |
+-----------------------------+-----------------------------+--------------------------+
| Python headers              | ``python-dev``              | use ``pyenv`` to install |
+-----------------------------+-----------------------------+--------------------------+

Please ``pip install numpy`` before installing turbodbc, because turbodbc will search
for the ``numpy`` Python package at installation/compile time. If NumPy is not installed,
turbodbc will not compile the optional NumPy support features.

(1) The minimum viable Boost setup requires the libraries ``variant``, ``optional``,
``datetime``, and ``locale``.


Windows
~~~~~~~

To install turbodbc on Windows, please use the following command:

::

    pip install turbodbc

This will download and install a binary wheel, no compilation required. You still need
to meet the following prerequisites, though:

+-------------+-----------------------------------------------+
| Requirement | Windows                                       |
+=============+===============================================+
| OS Bitness  | 64-bit                                        |
+-------------+-----------------------------------------------+
| Python      | 3.5 or 3.6, 64-bit                            |
+-------------+-----------------------------------------------+
| Runtime     | `MSVS 2015 Update 3 Redistributable, 64 bit`_ |
+-------------+-----------------------------------------------+

If you require NumPy support, please

::

    pip install numpy

Sometime after installing turbodbc.

.. _MSVS 2015 Update 3 Redistributable, 64 bit: https://www.microsoft.com/en-us/download/details.aspx?id=53840


Basic usage
-----------

Turbodbc follows the specification of the
`Python database API v2 (PEP 249) <https://www.python.org/dev/peps/pep-0249/>`_.
Here is a short summary, including the parts not specified by the PEP.

Establish a connection with your database
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All ODBC appications, including turbodbc, use connection strings to establish connections
with a database. If you know how the connection string for your database looks like,
use the following lines to establish a connection:

::

    >>> from turbodbc import connect
    >>> connection = connect(connection_string='Driver={PostgreSQL};Server=IP address;Port=5432;Database=myDataBase;Uid=myUsername;Pwd=myPassword;')

If you do not specify the ``connection_string`` keyword argument, turbodbc will create
a connection string based on the keyword arguments you pass to ``connect``:

::

    >>> from turbodbc import connect
    >>> connection = connect(dsn='My data source name as defined by your ODBC configuration')

The ``dsn`` is the data source name of your connection. Data source names uniquely identify
connection settings that shall be used to connect with a database. Data source names
are part of your :ref:`ODBC configuration <odbc_configuration>` and you need to set them up
yourself. Once set up, however, all ODBC applications can use the same data source name
to refer to the same set of connection options, typically including the host, database,
driver settings, and sometimes even credentials. If your ODBC environment is set up properly,
just using the ``dsn`` option should be sufficient.

You can add extra options besides the ``dsn`` to overwrite or add settings:

::

    >>> from turbodbc import connect
    >>> connection = connect(dsn='my dsn', user='my user has precedence')
    >>> connection = connect(dsn='my dsn', username='field names depend on the driver')

Last but not least, you can also do without a ``dsn`` and just specify all required configuration
options directly:

::

    >>> from turbodbc import connect
    >>> connection = connect(driver="PostgreSQL",
    ...                      server="hostname",
    ...                      port="5432",
    ...                      database="myDataBase",
    ...                      uid="myUsername",
    ...                      pwd="myPassword")


Executing SQL queries and retrieving results
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To execute a query, you need to create a ``cursor`` object first:

::

    >>> cursor = connection.cursor()

This cursor object lets you execute SQL commands and queries.
Here is how to execute a ``SELECT`` query:

::

    >>> cursor.execute('SELECT 42')

You have multiple options to retrieve the generated result set. For example, you can
iterate over the cursor:

::

    >>> for row in cursor:
    ...     print row
    [42L]

Alternatively, you can fetch all results as a list of rows:

::

    >>> cursor.fetchall()
    [[42L]]

You can also retrieve result sets as NumPy arrays, see :ref:`advanced_usage`.


Executing manipulating SQL queries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As before, you need to create a ``cursor`` object first:

::

    >>> cursor = connection.cursor()


You can now execute a basic ``INSERT`` query:

::

    >>> cursor.execute("INSERT INTO TABLE my_integer_table VALUES (42, 17)")

This will insert two values, ``42`` and ``17``, in a single row of table ``my_integer_table``.
Inserting values like this is impractical, because it requires to put the values
into the actual SQL string.

To avoid this, you can pass parameters to ``execute()``:

::

    >>> cursor.execute("INSERT INTO TABLE my_integer_table VALUES (?, ?)",
    ...                [42, 17])

Please note the question marks ``?`` in the SQL string that marks two parameters.
Adding single rows at a time is not efficient. You can add more than just a single row to a table
in efficiently by using ``executemany()``:

::

    >>> parameter_sets = [[42, 17],
    ...                   [23, 19],
    ...                   [314, 271]]
    >>> cursor.executemany("INSERT INTO TABLE my_integer_table VALUES (?, ?)",
    ...                    parameter_sets)


Transactions
~~~~~~~~~~~~

Turbodbc currently does not support autocommit. To commit your changes to the database,
please use the following command:

::

    >>> connection.commit()

If you want to roll back your changes, use the following command:

::

    >>> connection.rollback()


Supported data types
--------------------

Turbodbc supports the most common data types data scientists are interested in.
The following table shows which database types are converted to which Python types:

+-------------------------------------------+-----------------------+
| Database type(s)                          | Python type           |
+===========================================+=======================+
| Integers, ``DECIMAL(<19,0)``              | ``int``               |
+-------------------------------------------+-----------------------+
| ``DOUBLE``, ``DECIMAL(x, >0)``            | ``float``             |
+-------------------------------------------+-----------------------+
| ``BIT``, boolean-like                     | ``bool``              |
+-------------------------------------------+-----------------------+
| ``TIMESTAMP``, ``TIME``                   | ``datetime.datetime`` |
+-------------------------------------------+-----------------------+
| ``DATE``                                  | ``datetime.date``     |
+-------------------------------------------+-----------------------+
| ``VARCHAR``, strings, ``DECIMAL(>18, 0)`` | ``unicode`` (``str``) |
+-------------------------------------------+-----------------------+

When using parameters with ``execute()`` and ``executemany()``, the table is
basically reversed. The first type in the "database type(s)" column denotes
the type used to transfer back data. For integers, 64-bit integers are transferred.
For strings, the length of the transferred ``VARCHAR`` depends on the length of
the transferred strings.