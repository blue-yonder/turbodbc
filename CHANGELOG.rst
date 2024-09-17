Version history / changelog
===========================

From version 2.0.0, turbodbc adapts semantic versioning.

Version 5.0.0
-------------

* Switch to ``meson-python`` as the sole build system.
* Support only ``pyarrow>=14`` and have a hard dependency on it and ``numpy``.
* Switch to a single binary module.

Version 4.14.0
--------------

* Support ``pyarrow=2`

Version 4.13.0
--------------

* Support ``pyarrow=17`

Version 4.12.1
--------------

* Remove usage of ``pkg_resources``

Version 4.12.0
--------------

* Support ``pyarrow=16``
* Drop Python 3.9

Version 4.11.1
--------------

* Add upper pin for pyarrow in ``pyproject.toml``

Version 4.11.0
--------------

* Support ``pyarrow=15``

Version 4.10.0
--------------

* Fix performance bug in dealing with character columns under Windows, because of utf conversion by using `simdutf` instead of `wstring_convert`

Version 4.9.0
-------------

* Fix pip wheel missing pyproject.toml - support pep517

Version 4.8.0
-------------

* Support ``pyarrow=14``

Version 4.7.0
-------------

* Support ``pyarrow=13``

Version 4.6.0
--------------

* Drop python 3.8 support
* Add python 3.11 support
* Require ``numpy>=1.20.0``
* Require ``pyarrow>=7``
* Support ``pyarrow=12``
* Bump pybind11 version to 2.10.4
* Update GitHub Actions
* Update pre-commit hooks and linting

Version 4.5.10
--------------

* Support ``pyarrow=11``

Version 4.5.9
-------------

* Check for ``_GLIBCXX_USE_CXX11_ABI`` setting of ``pyarrow``.

Version 4.5.8
-------------

* Reupload of 4.5.7

Version 4.5.7
-------------

* Ensure we can correctly load ``arrow_python.dll`` on Windows.
* Various build system fixes as we now have macOS and Windows CI.
* Set ``_LIBCPP_DISABLE_AVAILABILITY`` to allow building with C++17 on macOS.
* Replace usage of ``boost::locale`` with their C++17 equivalents.

Version 4.5.6
-------------

* add in support and test for cursor.nextset() when dealing with multiple result sets
* modified the pyarrow logic in the setup.py to utilize the following helper
  functions
    * pyarrow.get_include()
    * pyarrow.get_libraries()
    * pyarrow.get_library_dirs()
* Bump pyarrow support to <11
    * Requires an additional pyarrow specific include directory
* Update the Microsoft ODBC Driver 17 for SQL Server version
* Bump c++11 to c++17 to enable pyarrow10 support
* Remove static lib logic for FindArrow

Version 4.5.5
-------------

* Update package requirements so that ``pyarrow>=1.0,<9.1`` can be used.

Version 4.5.4
-------------

* Update package requirements so that ``pyarrow>=1.0,<8.1`` can be used.

Version 4.5.3
-------------

* Bump version number to fix faulty release

Version 4.5.2
-------------

* ``oldest-supported-numpy`` is only for ``pyproject.toml``, thus we removed it from
  ``setup_requires`` again.

Version 4.5.1
-------------

* Bump version number to fix faulty release

Version 4.5.0
-------------

* Require ``pyarrow``, ``oldest-supported-numpy``, ``pybind11`` as ``setup_requires``
* Create ``pyarrow`` symlinks if ``pyarrow`` is available

Version 4.4.0
-------------

* Bump pybind11 version to 2.9.1
* Bump dependency requirements to ``pyarrow>=1.0`` and ``numpy>=1.19``
* Bump the used google test versions to 1.11.0

Version 4.3.1
-------------

* Update package requirements so that ``pyarrow>0.17.1,<7.1`` can be used.

Version 4.3.0
-------------

* Add Python 3.10 build 
* Update package requirements so that ``pyarrow>0.17.1,<6.1`` can be used.
* Bump pybind11 version to 2.8.1

Version 4.2.1
-------------

* Update package requirements so that ``pyarrow>0.17.1,<5.1`` can be used.

Version 4.2.0
-------------

* Update package requirements so that ``pyarrow>0.17.1,<4.1`` can be used.
* Set minimal Python version to 3.7 following NEP 29.

Version 4.1.2
-------------

* Update package requirements so ``pyarrow>0.17.1,<3.1`` can be used.

Version 4.1.1
-------------

* Version bump as package version was not adapted.

Version 4.1.0
-------------

* Adjust unit tests to Apache Arrow master changes.
* Bump bundled pybind11 to 2.5.0

Version 4.0.0
-------------

* Minimal supported python version is 3.6.X now

Version 3.4.0
-------------

* Support Arrow 0.16.x and 0.17.x, require at least 0.15.x
* Minimal NumPy version was bumped to 1.16

Version 3.3.0
-------------

* Don't override but amend ``CMAKE_CXX_FLAGS``
* Support Arrow 0.15.X

Version 3.2.0
-------------

* Release GIL while fetching batches using Apache Arrow engine
* Support Arrow 0.14.X


Version 3.1.1
-------------

* Correctly report odbc errors when freeing the statement handle as exceptions;
  see `Github issue 153 <https://github.com/blue-yonder/turbodbc/issues/153>`_
  (thanks @byjott)
* Support user-provided gmock/gtest, e.g. in conda environments via
  ``conda install -c conda-forge gtest gmock``.
* Make source code compatible with Apache Arrow 0.13.0

Version 3.1.0
-------------

*  Update to Apache Arrow 0.12
*  Support the unicode datatype in the Arrow support. This primarily enables
   MS SQL support for the Arrow adapter.
*  Windows support for the Arrow adapter.
*  Add a new entry to the build matrix that tests Python 3.7 with conda and
   MS SQL on Linux.
*  Big hands to @xhochy for making all these changes!

Version 3.0.0
-------------

*   Adjust generators to conform to PEP-479
*   Build wheels for Python 3.7 on Windows
*   Drop support for Python 3.4
*   Update to Apache Arrow 0.11

Version 2.7.0
-------------

*   Added new keyword argument ``fetch_wchar_as_char`` to ``make_options()``.
    If set to ``True``, wide character types (``NVARCHAR``) are fetched and
    decoded as narrow character types for compatibility with certain
    databases/drivers (thanks @yaxxie).
*   Added batched fetch support for Arrow as ``fetcharrowbatches()``
    (thanks @mariusvniekerk).
*   Support (u)int8, (u)int16, (u)int32 Arrow columns on
    ``executemanycolumns()`` (thanks @xhochy).

Version 2.6.0
-------------

*   Added support for ``with`` blocks for ``Cursor`` and ``Connection``
    objects. This makes turbodbc conform with
    `PEP 343 <https://www.python.org/dev/peps/pep-0343/>`_
    (thanks @AtomBaf)
*   Added new keyword argument ``force_extra_capacity_for_unicode`` to
    ``make_options()``. If set to ``True``, memory allocation is modified
    to operate under the assumption that the database driver reports field
    lengths in characters, rather than code units (thanks @yaxxie).
*   Updated Apache Arrow support to work with both versions 0.8.0 and 0.9.0
    (thanks @pacman82)
*   Fixed a bug that led to ``handle limit exceeded`` error messages when
    ``Cursor`` objects were not closed *manually*. With this fix, cursors
    are garbage collected as expected.

Version 2.5.0
-------------

*   Added an option to ``fetchallarrow()`` that fetches integer columns in the
    smallest possible integer type the retrieved values fit in. While this
    reduces the memory footprint of the resulting table, the schema of the
    table is now dependent on the data it contains.
*   Updated Apache Arrow support to work with version 0.8.x

Version 2.4.1
-------------

*   Fixed a memory leak on ``fetchallarrow()`` that increased the reference
    count of the returned table by one too much.

Version 2.4.0
-------------

*   Added support for Apache Arrow ``pyarrow.Table`` objects as the input for
    ``executemanycolumns()``. In addition to direct Arrow support, this
    should also help with more graceful handling of Pandas DataFrames
    as ``pa.Table.from_pandas(...)`` handles additional corner cases of
    Pandas data structures. Big thanks to @xhochy!

Version 2.3.0
-------------

*   Added an option to ``fetchallarrow()`` that enables the fetching of string
    columns as dictionary-encoded string columns. In most cases, this increases
    performance and reduces RAM usage. Arrow columns of type ``dictionary[string]``
    will result in ``pandas.Categorical`` columns on conversion.
*   Updated pybind11 dependency to version 2.2+
*   Fixed a symbol visibility issue when building Arrow unit tests on systems
    that hide symbols by default.

Version 2.2.0
-------------

*   Added new keyword argument ``large_decimals_as_64_bit_types`` to
    ``make_options()``. If set to ``True``, decimals with more than ``18``
    digits will be retrieved as 64 bit integers or floats as appropriate.
    The default retains the previous behavior of returning strings.
*   Added support for ``datetime64[ns]`` data type for ``executemanycolumns()``.
    This is particularly helpful when dealing with `pandas <https://pandas.pydata.org>`_
    ``DataFrame`` objects, since this is the type that contains time stamps.
*   Added the keyword argument ``limit_varchar_results_to_max`` to ``make_options()``. This
    allows to truncate ``VARCHAR(n)`` fields to ``varchar_max_character_limit``
    characters, see the next item.
*   Added possibility to enforce NumPy and Apache Arrow requirements using extra requirements
    during installation: ``pip install turbodbc[arrow,numpy]``
*   Updated Apache Arrow support to work with version 0.6.x
*   Fixed an issue with retrieving result sets with ``VARCHAR(max)`` fields and
    similar types. The size of the buffer allocated for such fields can be controlled
    with the ``varchar_max_character_limit`` option to ``make_options()``.
*   Fixed an `issue with some versions of Boost <https://svn.boost.org/trac10/ticket/3471>`_
    that lead to problems with ``datetime64[us]`` columns with ``executemanycolumns()``.
    An overflow when converting microseconds since 1970 to a database-readable timestamp
    could happen, badly garbling the timestamps in the process. The issue was
    surfaced with Debian 7's Boost version (1.49), although the Boost
    issue was allegedly fixed with version 1.43.
*   Fixed an issue that lead to undefined behavior when character sequences
    could not be decoded into Unicode code points. The new (and defined) behavior
    is to ignore the offending character sequences completely.


Version 2.1.0
-------------

*   Added new method ``cursor.executemanycolumns()`` that accepts parameters
    in columnar fashion as a list of NumPy (masked) arrays.
*   CMake build now supports ``conda`` environments
*   CMake build offers ``DISABLE_CXX11_ABI`` option to fix linking issues
    with ``pyarrow`` on systems with the new C++11 compliant ABI enabled

Version 2.0.0
-------------

*   Initial support for the arrow data format with the ``Cursor.fetchallarrow()``
    method. Still in alpha stage, mileage may vary (Windows not yet supported,
    UTF-16 unicode not yet supported). Big thanks to @xhochy!
*   ``prefer_unicode`` option now also affects column name rendering
    when gathering results from the database. This effectively enables
    support for Unicode column names for some databases.
*   Added module version number ``turbodbc.__version__``
*   Removed deprecated performance options for ``connect()``. Use
    ``connect(..., turbodbc_options=make_options(...))`` instead.

Earlier versions (not conforming to semantic versioning)
--------------------------------------------------------

The following versions do not conform to semantic versioning. The
meaning of the ``major.minor.revision`` versions is:

*   Major: psychological ;-)
*   Minor: If incremented, this indicates a breaking change
*   Revision: If incremented, indicates non-breaking change (either feature or bug fix)

Version 1.1.2
-------------

*   Added ``autocommit`` as a keyword argument to ``make_options()``. As the
    name suggests, this allows you to enable automatic ``COMMIT`` operations
    after each operation. It also improves compatibility with databases
    that do not support transactions.
*   Added ``autocommit`` property to ``Connection`` class that allows switching
    autocommit mode after the connection was created.
*   Fixed bug with ``cursor.rowcount`` not being reset to ``-1`` when calls to
    ``execute()`` or ``executemany()`` raised exceptions.
*   Fixed bug with ``cursor.rowcount`` not showing the correct value when
    manipulating queries were used without placeholders, i.e., with
    parameters baked into the query.
*   Global interpreter lock (GIL) is released during some operations to
    facilitate basic multi-threading (thanks @chmp)
*   Internal: The return code ``SQL_SUCCESS_WITH_INFO`` is now treated as
    a success instead of an error when allocating environment, connection,
    and statement handles. This may improve compatibility with some databases.

Version 1.1.1
-------------

*   Windows is now _officially_ supported (64 bit, Python 3.5 and 3.6). From now on,
    code is automatically compiled and tested on Linux, OSX, and Windows
    (thanks @TWAC for support). Windows binary wheels are uploaded to pypi.
*   Added supported for fetching results in batches of NumPy objects with
    ``cursor.fetchnumpybatches()`` (thanks @yaxxie)
*   MSSQL is now part of the Windows test suite (thanks @TWAC)
*   ``connect()`` now allows to specify a ``connection_string`` instead of
    individual arguments that are then compiles into a connection string (thanks @TWAC).

Version 1.1.0
-------------

*   Added support for databases that require Unicode data to be transported
    in UCS-2/UCS-16 format rather than UTF-8, e.g., MSSQL.
*   Added _experimental_ support for Windows source distribution builds.
    Windows builds are not fully (or automatically) tested yet, and still require
    significant effort on the user side to compile (thanks @TWAC for this initial version)
*   Added new ``cursor.fetchnumpybatches()`` method which returns a generator to
    iterate over result sets in batch sizes as defined by buffer size or rowcount
    (thanks @yaxxie)
*   Added ``make_options()`` function that take all performance and compatibility
    settings as keyword arguments.
*   Deprecated all performance options (``read_buffer_size``, ``use_async_io``, and
    ``parameter_sets_to_buffer``) for ``connect()``. Please move these keyword arguments
    to ``make_options()``. Then, set ``connect{}``'s new keyword argument ``turbodbc_options``
    to the result of ``make_options()``. This effectively separates performance options
    from options passed to the ODBC connection string.
*   Removed deprecated option ``rows_to_buffer`` from ``turbodbc.connect()``
    (see version 0.4.1 for details).
*   The order of arguments for ``turbodbc.connect()`` has changed; this may affect
    you if you have not used keyword arguments.
*   The behavior of ``cursor.fetchallnumpy()`` has changed a little. The
    ``mask`` attribute of a generated ``numpy.MaskedArray`` instance is
    shortened to ``False`` from the previous ``[False, ..., False]`` if the
    mask is ``False`` for all entries. This can cause problems when you
    access individual indices of the mask.
*   Updated ``pybind11`` requirement to at least ``2.1.0``.
*   Internal: Some types have changed to accomodate for Linux/OSX/Windows compatibility.
    In particular, a few ``long`` types were converted to ``intptr_t`` and ``int64_t``
    where appropriate. In particular, this affects the ``field`` type that may be used
    by C++ end users (so they exist).


Version 1.0.5
-------------

*   Internal: Remove some ``const`` pointers to resolve some compile issues with
    xcode 6.4 (thanks @xhochy)

Version 1.0.4
-------------

*   Added possibility to set unixodbc include and library directories in
    setup.py. Required for conda builds.

Version 1.0.3
-------------

*   Improved compatibility with ODBC drivers (e.g. FreeTDS) that do not
    support ODBC's ``SQLDescribeParam()`` function by using a default
    parameter type.
*   Used a default parameter type when the ODBC driver cannot determine
    a parameter's type, for example when using column expressions for
    ``INSERT`` statements.
*   Improved compatibility with some ODBC drivers (e.g. Microsoft's official
    MSSQL ODBC driver) for setting timestamps with fractional seconds.

Version 1.0.2
-------------

*   Added support for chaining operations to ``Cursor.execute()`` and
    ``Cursor.executemany()``. This allows one-liners such as
    ``cursor.execute("SELECT 42").fetchallnumpy()``.
*   Right before a database connection is closed, any open transactions
    are explicitly rolled back. This improves compatibility with ODBC drivers
    that do not perform automatic rollbacks such as Microsoft's official
    ODBC driver.
*   Improved stability of turbodbc when facing errors while closing connections,
    statements, and environments. In earlier versions, connection timeouts etc.
    could have lead to the Python process's termination.
*   Source distribution now contains license, readme, and changelog.

Version 1.0.1
-------------

*   Added support for OSX

Version 1.0.0
-------------

*   Added support for Python 3. Python 2 is still supported as well.
    Tested with Python 2.7, 3.4, 3.5, and 3.6.
*   Added ``six`` package as dependency
*   Turbodbc uses pybind11 instead of Boost.Python to generate its Python
    bindings. pybind11 is available as a Python package and automatically
    installed when you install turbodbc.
    Other boost libraries are still required for other aspects of the code.
*   A more modern compiler is required due to the pybind11 dependency.
    GCC 4.8 will suffice.
*   Internal: Move remaining stuff depending on python to turbodbc_python
*   Internal: Now requires CMake 2.8.12+ (get it with ``pip install cmake``)

Version 0.5.1
-------------

*   Fixed build issue with older numpy versions, e.g., 1.8 (thanks @xhochy)

Version 0.5.0
-------------

*   Improved performance of parameter-based operations.
*   Internal: Major modifications to the way parameters are handled.

Version 0.4.1
-------------

*   The size of the input buffers for retrieving result sets can now be set
    to a certain amount of memory instead of using a fixed number of rows.
    Use the optional ``read_buffer_size`` parameter of ``turbodbc.connect()`` and
    set it to instances of the new top-level classes ``Megabytes`` and ``Rows``
    (thanks @LukasDistel).
*   The read buffer size's default value has changed from 1,000 rows to
    20 MB.
*   The parameter ``rows_to_buffer`` of ``turbodbc.connect()`` is _deprecated_.
    You can set the ``read_buffer_size`` to ``turbodbc.Rows(1000)`` for the same
    effect, though it is recommended to specify the buffer size in MB.
*   Internal: Libraries no longer link ``libpython.so`` for local development
    (linking is already done by the Python interpreter). This was always
    the case for the libraries in the packages uploaded to PyPI, so no
    change was necessary here.
*   Internal: Some modifications to the structure of the underlying
    C++ code.

Version 0.4.0
-------------

*   NumPy support is introduced to turbodbc for retrieving result sets.
    Use ``cursor.fetchallnumpy`` to retrieve a result set as an ``OrderedDict``
    of ``column_name: column_data`` pairs, where ``column_data`` is a NumPy ``MaskedArray``
    of appropriate type.
*   Internal: Single ``turbodbc_intern`` library was split up into three libraries
    to keep NumPy support optional. A few files were moved because of this.

Version 0.3.0
-------------

*   turbodbc now supports asynchronous I/O operations for retrieving result sets.
    This means that while the main thread is busy converting an already retrieved
    batch of results to Python objects, another thread fetches an additional
    batch in the background. This may yield substantial performance improvements
    in the right circumstances (results are retrieved in roughly the same speed
    as they are converted to Python objects).

    Ansynchronous I/O support is experimental. Enable it with
    ``turbodbc.connect('My data source name', use_async_io=True)``

Version 0.2.5
-------------

*   C++ backend: ``turbodbc::column`` no longer automatically binds on
    construction. Call ``bind()`` instead.

Version 0.2.4
-------------

*   Result set rows are returned as native Python lists instead of a not easily
    printable custom type.
*   Improve performance of Python object conversion while reading result sets.
    In tests with an Exasol database, performance got about 15% better.
*   C++ backend: ``turbodbc::cursor`` no longer allows direct access to the C++
    ``field`` type. Instead, please use the ``cursor``'s ``get_query()`` method,
    and construct a ``turbodbc::result_sets::field_result_set`` using the
    ``get_results()`` method.

Version 0.2.3
-------------

*   Fix issue that only lists were allowed for specifying parameters for queries
*   Improve parameter memory consumption when the database reports very large
    string parameter sizes
*   C++ backend: Provides more low-level ways to access the result set

Version 0.2.2
-------------

*   Fix issue that ``dsn`` parameter was always present in the connection string
    even if it was not set by the user's call to ``connect()``
*   Internal: First version to run on Travis.
*   Internal: Use pytest instead of unittest for testing
*   Internal: Allow for integration tests to run in custom environment
*   Internal: Simplify integration test configuration


Version 0.2.1
-------------

*   Internal: Change C++ test framework to Google Test


Version 0.2.0
-------------

*   New parameter types supported: ``bool``, ``datetime.date``, ``datetime.datetime``
*   ``cursor.rowcount`` returns number of affected rows for manipulating queries
*   ``Connection`` supports ``rollback()``
*   Improved handling of string parameters


Version 0.1.0
-------------

Initial release
