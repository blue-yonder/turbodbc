Version 1.1.0 (unreleased)
==========================
*   Added support for databases that require Unicode data to be transported
    in UCS-2/UCS-16 format rather than UTF-8, e.g., MSSQL.
*   Added `make_options()` function that take all performance and compatibility
    settings as keyword arguments.
*   Deprecated all performance options (`read_buffer_size`, `use_async_io`, and
    `parameter_sets_to_buffer`) for `connect()`. Please move these keyword arguments
    to `make_options()`. Then, set `connect{}`'s new keyword argument `turbodbc_options`
    to the result of `make_options()`. This effectively separates performance options
    from options passed to the ODBC connection string.
*   Removed deprecated option `rows_to_buffer` from `turbodbc.connect()`
    (see version 0.4.1 for details).
*   The order of arguments for `turbodbc.connect()` has changed; this may affect
    you if you have not used keyword arguments.

Version 1.0.5
=============
*   Internal: Remove some `const` pointer to resolve some compile issues with
    xcode 6.4 (thanks @xhochy)

Version 1.0.4
=============
*   Added possibility to set unixodbc include and library directories in
    setup.py. Required for conda builds.

Version 1.0.3
=============
*   Improved compatibility with ODBC drivers (e.g. FreeTDS) that do not
    support ODBC's `SQLDescribeParam()` function by using a default
    parameter type.
*   Used a default parameter type when the ODBC driver cannot determine
    a parameter's type, for example when using column expressions for
    `INSERT` statements.
*   Improved compatibility with some ODBC drivers (e.g. Microsoft's official
    MSSQL ODBC driver) for setting timestamps with fractional seconds.

Version 1.0.2
=============
*   Added support for chaining operations to `Cursor.execute()` and
    `Cursor.executemany()`. This allows one-liners such as
    `cursor.execute("SELECT 42").fetchallnumpy()`.
*   Right before a database connection is closed, any open transactions
    are explicitly rolled back. This improves compatibility with ODBC drivers
    that do not perform automatic rollbacks such as Microsoft's official
    ODBC driver.
*   Improved stability of turbodbc when facing errors while closing connections,
    statements, and environments. In earlier versions, connection timeouts etc.
    could have lead to the Python process's termination.
*   Source distribution now contains license, readme, and changelog.

Version 1.0.1
=============
*   Added support for OSX

Version 1.0.0
=============
*   Added support for Python 3. Python 2 is still supported as well.
    Tested with Python 2.7, 3.4, 3.5, and 3.6.
*   Added `six` package as dependency
*   Turbodbc uses pybind11 instead of Boost.Python to generate its Python
    bindings. pybind11 is available as a Python package and automatically
    installed when you install turbodbc.
    Other boost libraries are still required for other aspects of the code.
*   A more modern compiler is required due to the pybind11 dependency.
    GCC 4.8 will suffice.
*   Internal: Move remaining stuff depending on python to turbodbc_python
*   Internal: Now requires cmake 2.8.12+ (get it with `pip install cmake`)

Version 0.5.1
=============
*   Fixed build issue with older numpy versions, e.g., 1.8 (thanks @xhochy)

Version 0.5.0
=============
*   Improved performance of parameter-based operations.
*   Internal: Major modifications to the way parameters are handled.

Version 0.4.1
=============
*   The size of the input buffers for retrieving result sets can now be set
    to a certain amount of memory instead of using a fixed number of rows.
    Use the optional `read_buffer_size` parameter of `turbodbc.connect()` and
    set it to instances of the new top-level classes `Megabytes` and `Rows`
    (thanks @LukasDistel).
*   The read buffer size's default value has changed from 1,000 rows to
    20 MB.
*   The parameter `rows_to_buffer` of `turbodbc.connect()` is _deprecated_.
    You can set the `read_buffer_size` to `turbodbc.Rows(1000)` for the same
    effect, though it is recommended to specify the buffer size in MB.
*   Internal: Libraries no longer link `libpython.so` for local development
    (linking is already done by the Python interpreter). This was always
    the case for the libraries in the packages uploaded to PyPI, so no
    change was necessary here.
*   Internal: Some modifications to the structure of the underlying 
    C++ code.

Version 0.4.0
=============

*   NumPy support is introduced to turbodbc for retrieving result sets.
    Use `cursor.fetchallnumpy` to retrieve a result set as an `OrderedDict`
    of `column_name: column_data` pairs, where `column_data` is a NumPy `MaskedArray`
    of appropriate type.
*   Internal: Single `turbodbc_intern` library was split up into three libraries
    to keep NumPy support optional. A few files were moved because of this.

Version 0.3.0
=============

*   turbodbc now supports asynchronous I/O operations for retrieving result sets.
    This means that while the main thread is busy converting an already retrieved
    batch of results to Python objects, another thread fetches an additional
    batch in the background. This may yield substantial performance improvements
    in the right circumstances (results are retrieved in roughly the same speed
    as they are converted to Python objects).
    
    Ansynchronous I/O support is experimental. Enable it with
    `turbodbc.connect('My data source name', use_async_io=True)`

Version 0.2.5
=============

*   C++ backend: `turbodbc::column` no longer automatically binds on
    construction. Call `bind()` instead.

Version 0.2.4
=============

*   Result set rows are returned as native Python lists instead of a not easily
    printable custom type.
*   Improve performance of Python object conversion while reading result sets.
    In tests with an Exasol database, performance got about 15% better.
*   C++ backend: `turbodbc::cursor` no longer allows direct access to the C++
    `field` type. Instead, please use the `cursor`'s `get_query()` method,
    and construct a `turbodbc::result_sets::field_result_set` using the
    `get_results()` method.

Version 0.2.3
=============

*   Fix issue that only lists were allowed for specifying parameters for queries
*   Improve parameter memory consumption when the database reports very large
    string parameter sizes 
*   C++ backend: Provides more low-level ways to access the result set

Version 0.2.2
=============

*   Fix issue that `dsn` parameter was always present in the connection string
    even if it was not set by the user's call to `connect()`
*   Internal: First version to run on Travis.
*   Internal: Use pytest instead of unittest for testing
*   Internal: Allow for integration tests to run in custom environment
*   Internal: Simplify integration test configuration


Version 0.2.1
=============

*   Internal: Change C++ test framework to Google Test


Version 0.2.0
=============

*   New parameter types supported: `bool`, `datetime.date`, `datetime.datetime`
*   `cursor.rowcount` returns number of affected rows for manipulating queries
*   `Connection` supports `rollback()`
*   Improved handling of string parameters


Version 0.1.0
=============

Initial release