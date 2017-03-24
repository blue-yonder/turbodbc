![turbodbc logo](/page/logo.png?raw=true "turbodbc logo")

Turbodbc - Turbocharged database access for data scientists.
============================================================

[![Build Status](https://travis-ci.org/blue-yonder/turbodbc.svg?branch=master)](https://travis-ci.org/blue-yonder/turbodbc)

Turbodbc is a Python module to access relational databases via the Open Database
Connectivity (ODBC) interface. In addition to complying with the Python Database API
Specification 2.0, turbodbc offers built-in NumPy support. Don't wait minutes for your
results, just blink.

Features
--------

*   Bulk retrieval of result sets
*   Built-in NumPy conversion
*   Bulk transfer of query parameters
*   Asynchronous I/O for result sets
*   Automatic conversion of decimal type to integer, float, and string as
    appropriate
*   Supported data types for both result sets and parameters:
    `int`, `float`, `str`, `bool`, `datetime.date`, `datetime.datetime`
*   Also provides a high-level C++11 database driver under the hood
*   Tested with Python 2.7, 3.4, 3.5, and 3.6
*   Tested on Linux and OSX


Installation
------------

To install turbodbc, please use the following command:

    pip install turbodbc

If you want to leverage turbodbc's NumPy support, please make sure to `pip install numpy`
before installing turbodbc, since turbodbc searches for NumPy headers
at installation time to determine whether NumPy support can be provided.

Since turbodbc includes C-extensions, make sure the following prerequisites
are given:

Requirement                 | Linux (`apt-get install`) | OSX (`brew install`)   | Windows (experimental) |
----------------------------|---------------------------|------------------------|------------------------|
C++11 compiler              | G++-4.8 or higher         | clang with OSX 10.9+   | Visual C++ 2015        |
Boost library + headers (1) | `libboost-all-dev`        | `boost`                | Boost sources (2)      |
ODBC library + headers      | `unixodbc-dev`            | `unixodbc`             | Windows Kits           |
Python headers              | `python-dev`              | use `pyenv` to install | Python 3.5+ (3)        |

(1) The minimum viable Boost setup requires the libraries `variant`, `optional`,
`datetime`, and `locale`.

(2) After unpacking Boost run Simplified Build From Source (`bootstrap` and `.\b2`).
Also make sure the enviroment variable `BOOST_ROOT` is pointing to the installation.

(3) Python 2.7 and 3.4 uses older compilers on Windows, and can not be used.


Why should I use turbodbc instead of other ODBC modules?
--------------------------------------------------------

Short answer: turbodbc is faster.

Slightly longer answer: turbodbc is faster, _much_ faster if you want to
work with NumPy.

Medium-length answer: I have tested turbodbc and pyodbc (probably the most
popular Python ODBC module) with various databases (Exasol, PostgreSQL, MySQL)
and corresponding ODBC drivers. I found turbodbc to be consistently faster.

For retrieving result sets, I found speedups between 1.5 and 7 retrieving plain
Python objects. For inserting data, I found speedups of up to 100.

Is this completely scientific? Not at all. I have not told you about which
hardware I used, which operating systems, drivers, database versions, network
bandwidth, database layout, SQL queries, what is measured, and how I performed
was measured.

All I can tell you is that if I exchange pyodbc with turbodbc, my benchmarks
took less time, often approaching one (reading) or two (writing) orders of
magnitude. Give it a spin for yourself, and tell me if you liked it.


Smooth. What is the trick?
--------------------------

Turbodbc exploits buffering.

* Turbodbc implements both sending parameters and retrieving result sets using
buffers of multiple rows/parameter sets. This avoids round trips to the ODBC
driver and (depending how well the ODBC driver is written) to the database.
* Multiple buffers are used for asynchronous I/O. This allows to interleave
Python object conversion and direct database interaction (see performance options
below).
* Buffers contain binary representations of data. NumPy arrays contain binary
representations of data. Good thing they are often the same, so instead of
converting we can just copy data.


Supported data types
--------------------

The following data types are supported:

Database type(s)                  | Python type      | NumPy type    | Notes
----------------------------------|------------------|---------------|-------
integers, Decimal(<19,0)          | `int`             | `int64`       |
floating point, Decimal(x, >0)    | `float`           | `float64`      |
bit, boolean-like                 | `bool`            | `bool_`        |
timestamp, time                   | `datetime.datetime` | `datetime64[us]` |
date                              | `datetime.date`    | `datetime64[D]`  |
strings, VARCHAR, Decimal(>18, 0) | `unicode`          | `object_`      | _slow, WIP_

NumPy types are not yet supported for parameters.


Basic usage
-----------

Turbodbc follows the specification of the Python database API v2, which you can
find at https://www.python.org/dev/peps/pep-0249/. Here is a short summary,
including the parts not specified.

To establish a connection, use any of the following commands:

    >>> from turbodbc import connect
    >>> connection = connect(dsn='My data source name as given by odbc.ini')
    >>> connection = connect(dsn='my dsn', user='my user has precedence')
    >>> connection = connect(dsn='my dsn', username='field names may depend on the driver')

To execute a query, you need a `cursor` object:

    >>> cursor = connection.cursor()

Here is how to execute a `SELECT` query:

    >>> cursor.execute('SELECT 42')
    >>> for row in cursor:
    >>>     print list(row)

Here is how to execute an `INSERT` query with many parameters:

    >>> parameter_sets = [['hi', 42],
                          ['there', 23]]
    >>> cursor.executemany('INSERT INTO my_table VALUES (?, ?)',
                           parameter_sets)


NumPy support
-------------

Here is how to retrieve a result set in the form of NumPy arrays:

    >>> cursor.execute("SELECT A, B FROM my_table")
    >>> cursor.fetchallnumpy()
    OrderedDict([('A', masked_array(data = [42 --],
                                    mask = [False True],
                                    fill_value = 999999)),
                 ('B', masked_array(data = [3.14 2.71],
                                    mask = [False False],
                                    fill_value = 1e+20))])

Please note a few things:

*   The return value is an `OrderedDict` of column name/value pairs. The column
    order is the same as in your query.
*   The column values are `MaskedArray`s. Any `NULL` values you have in your
    database will show up as masked entries (`NULL` values in string-like columns
    will shop up as `None` objects).
*   NumPy support is limited to result sets, experimental, and will probably change
    with the next iterations.

Performance and compatibility options
-------------------------------------

Turbodbc offers a way to adjust its behavior to tune performance and to
achieve compatibility with your database. The basic usage is this:

    >>> from turbodbc import connect, make_options
    >>> options = make_options()
    >>> connect(dsn="my_dsn", turbodbc_options=options)

This will connect with your database using the default options. To use non-default
options, supply keyword arguments to `make_options()`:

    >>> from turbodbc import Megabytes
    >>> options = make_options(read_buffer_size=Megabytes(100),
                               parameter_sets_to_buffer=1000,
                               use_async_io=True,
                               prefer_unicode=True)

`read_buffer_size` affects how many result set rows are retrieved per batch
of results. Set the attribute to `turbodbc.Megabytes(42)` to have turbodbc determine
the optimal number of rows per batch so that the total buffer amounts to
42 MB. This is recommended for most users and databases. You can also set
the attribute to `turbodbc.Rows(13)` if you would like to fetch results in
batches of 13 rows. By default, turbodbc fetches results in batches of 20 MB.

Similarly, `parameter_sets_to_buffer` changes the number of parameter sets
which are transferred per batch of parameters (e.g., as sent with `executemany()`).
Please note that it is not (yet) possible to use the `Megabytes` and `Rows` classes
here.

If you set `use_async_io` to `True`, turbodbc will use asynchronous I/O operations
(limited to result sets for the time being). Asynchronous I/O means that while the
main thread converts result set rows retrieved from the database to Python
objects, another thread fetches a new batch of results from the database in the background. This may yield
significant speedups when retrieving and converting are similarly fast
operations.

    Asynchronous I/O is experimental and has to fully prove itself yet.
    Do not be afraid to give it a try, though.

Finally, set `prefer_unicode` to `True` if your database does not fully support
the UTF-8 encoding turbodbc prefers. With this option you can tell turbodbc
to use two-byte character strings with UCS-2/UTF-16 encoding. Use this option
if you try to connection to Microsoft SQL server (MSSQL).


Development version
-------------------

To use the latest version of turbodbc, do the following.

1.  Create a Python virtual environment, activate it, and install the necessary
    packages numpy, pytest, and mock:

        pip install numpy pytest mock

1.  Clone turbodbc into the virtual environment somewhere:

        git clone https://github.com/blue-yonder/turbodbc.git

1.  `cd` into the git repo and pull in the `pybind11` submodule by running:

        git submodule update --init --recursive

1.  Check the source build requirements (see below) are installed on your
    computer.
1.  Create a build directory somewhere and `cd` into it.
1.  Execute the following command:

        cmake -DCMAKE_INSTALL_PREFIX=./dist /path/to/turbodbc

    where the final path parameter is the directory to the turbodbc git repo,
    specifically the directory containing `setup.py`. This `cmake` command will
    prepare the build directory for the actual build step.

1.  Run `make`. This will build (compile) the source code.
1.  At this point you can run the test suite. First, make a copy of the
    relevant json documents from the turbodbc `python/turbodbc_test` directory,
    there's one for each database. Then edit your copies with the relevant
    credentials. Next, set the environment variable TURBODBC_TEST_CONFIGURATION_FILES
    as a comma-separated list of the json files you've just copied and run
    the test suite, as follows:

        export TURBODBC_TEST_CONFIGURATION_FILES="<MySql json file>,<Postgres json file>,<MS SQL json file>"
        ctest --output-on-failure

1.  Finally, to create a Python source distribution for `pip` installation, run
    the following from the build directory:

        make install
        cd dist
        python setup.py sdist

    This will create a `turbodbc-x.y.z.tar.gz` file locally which can be used
    by others to install turbodbc with `pip install turbodbc-x.y.z.tar.gz`.

Source build requirements
-------------------------

For the development build, you also require the following additional
dependencies:

*   CMake


Supported environments
----------------------

*   Linux (successfully built on Ubuntu 12, Ubuntu 14, Debian 7, Debian 8)
*   OSX (successfully built on Sierra a.k.a. 10.12 and El Capitan a.k.a. 10.11)
*   Python 2.7, 3.4, 3.5, 3.6
*   More environments probably work as well, but these are the versions that
    are regularly tested on Travis or local development machines.


Supported databases
-------------------

Turbodbc uses suites of unit and integration tests to ensure quality.
Every time turbodbc's code is updated on GitHub,
turbodbc is automatically built from scratch and tested with the following databases:

*   PostgreSQL
*   MySQL

During development, turbodbc is tested with the following database:

*   Exasol

Releases will not be made if any (implemented) test fails for any of the databases
listed above. In addition to these well-supported databases, the following databases
are tested on an irregular basis:

*   MSSQL with FreeTDS
*   MSSQL with Microsoft's official ODBC driver

These database/driver combinations do not yet pass all tests.

There is a good chance that turbodbc will work with other, totally untested databases
as well. There is, however, an equally good chance that you will encounter compatibility
issues. If you encounter one, please take the time to report it so turbodbc can be improved
to work with more real-world databases. Thanks!


SQLAlchemy support
------------------

Using Turbodbc in combination with SQLAlchemy is possible for a limited number of databases:

* Exasol: [sqlalchemy_exasol](https://github.com/blue-yonder/sqlalchemy_exasol)
* MSSQL: [sqlalchemy-turbodbc](https://github.com/dirkjonker/sqlalchemy-turbodbc) (experimental)


I got questions and issues to report!
-------------------------------------

In this case, please use turbodbc's issue tracker on GitHub.


Is there a guided tour through turbodbc's entrails?
---------------------------------------------------

Yes, there is! Check out this blog post on
[the making of turbodbc](http://tech.blue-yonder.com/making-of-turbodbc-part-1-wrestling-with-the-side-effects-of-a-c-api/).


Is turbodbc on Twitter?
-----------------------

Yes, it is! Just follow [@turbodbc](https://twitter.com/turbodbc)
for the latest turbodbc talk and news about related technologies.
