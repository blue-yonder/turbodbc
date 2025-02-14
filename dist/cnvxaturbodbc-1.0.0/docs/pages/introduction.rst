Introduction
============

Turbodbc is a Python module to access relational databases via the Open Database
Connectivity (ODBC) interface. In addition to complying with the
`Python Database API Specification 2.0 <https://www.python.org/dev/peps/pep-0249/>`_,
turbodbc offers built-in NumPy support. Don't wait minutes for your results, just blink.


Features
--------

*   Bulk retrieval of result sets
*   Built-in NumPy support
*   Built-in Apache Arrow support
*   Bulk transfer of query parameters
*   Asynchronous I/O for result sets
*   Automatic conversion of decimal type to integer, float, and string as
    appropriate
*   Supported data types for both result sets and parameters:
    ``int``, ``float``, ``str``, ``bool``, ``datetime.date``, ``datetime.datetime``
*   Also provides a high-level C++11 database driver under the hood
*   Tested with Python 3.9, 3.10 and 3.11
*   Tested on 64 bit versions of Linux, OSX, and Windows (Python 3.9+).


Why should I use turbodbc instead of other ODBC modules?
--------------------------------------------------------

Short answer: turbodbc is faster.

Slightly longer answer: turbodbc is faster, *much* faster if you want to
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

*   Turbodbc implements both sending parameters and retrieving result sets using
    buffers of multiple rows/parameter sets. This avoids round trips to the ODBC
    driver and (depending how well the ODBC driver is written) to the database.
*   Multiple buffers are used for asynchronous I/O. This allows to interleave
    Python object conversion and direct database interaction (see performance options
    below).
*   Buffers contain binary representations of data. NumPy arrays contain binary
    representations of data. Good thing they are often the same, so instead of
    converting we can just copy data.


Supported environments
----------------------

*   64 bit operating systems (32 bit not supported)
*   Linux (successfully built on Ubuntu 12, Ubuntu 14, Debian 7, Debian 8)
*   OSX (successfully built on Sierra a.k.a. 10.12 and El Capitan a.k.a. 10.11)
*   Windows (successfully built on Windows 10)
*   Python 3.9, 3.10, 3.11
*   More environments probably work as well, but these are the versions that
    are regularly tested on Travis or local development machines.


Supported databases
-------------------

Turbodbc uses suites of unit and integration tests to ensure quality.
Every time turbodbc's code is updated on GitHub,
turbodbc is automatically built from scratch and tested with the following databases:

*   PostgreSQL (Linux, OSX, Windows)
*   MySQL (Linux, OSX, Windows)
*   MSSQL (Windows, with official MS driver)

During development, turbodbc is tested with the following database:

*   Exasol (Linux, OSX)

Releases will not be made if any (implemented) test fails for any of the databases
listed above. The following databases/driver combinations are tested on an irregular
basis:

*   MSSQL with FreeTDS (Linux, OSX)
*   MSSQL with Microsoft's official ODBC driver (Linux)

There is a good chance that turbodbc will work with other, totally untested databases
as well. There is, however, an equally good chance that you will encounter compatibility
issues. If you encounter one, please take the time to report it so turbodbc can be improved
to work with more real-world databases. Thanks!
