Troubleshooting
===============

This section contains advice on how to troubleshoot ODBC connections.
The advice contained here is not specific to turbodbc, but very related.

.. note::
    This section currently assumes you are on a Linux/OSX machine that uses
    unixodbc as a :ref:`driver manager <odbc_driver_manager>`. Windows users
    may find the contained information useful, but should expect some additional
    transfer work adjusting the advice to the Windows platform.


Testing your ODBC configuration
-------------------------------

You can test your configuration with turbodbc, obviously, by creating a connection.
It is preferable, however, to use the tool ``isql`` that is shipped together with
``unixodbc``. It is a very simple program that does not try anything fancy and is
perfectly suited for debugging. If you configuration does not work with ``isql``,
it will not work with turbodbc.

.. note::

    Before you file an issue with turbodbc, please make sure that you can actually
    connect to your database using ``isql``.

When you have selected an ODBC configuration as outlined above, enter the following
command in a shell:

::

    > isql "data source name" user password -v

Specifying user and password is optional. On success, this will output a shell such as this:

::

    +---------------------------------------+
    | Connected!                            |
    |                                       |
    | sql-statement                         |
    | help [tablename]                      |
    | quit                                  |
    |                                       |
    +---------------------------------------+
    SQL>

You can type in any SQL command you wish to test or leave the shell with the ``quit``
command. In case of errors, a hopefully somewhat helpful error message will be displayed.


Common configuration errors
---------------------------

::

    [IM002][unixODBC][Driver Manager]Data source name not found, and no default driver specified
    [ISQL]ERROR: Could not SQLConnect

This usually means that the data source name could not be found because the configuration is
not active. Troubleshooting:

*   Check for typos in data source names in ``odbc.ini`` or your shell.
*   Check if the correct ``odbc.ini`` file is used.
*   Check the values of ``$ODBCSYSINI`` and ``$ODBCINI`` (usually only one should be set).
*   Check if ``$ODBCINSTINI`` is set (usually it should not be set). Unset the variable.
*   Check your data source has a ``Driver`` section.


::

    [01000][unixODBC][Driver Manager]Can't open lib '/path/to/driver.so' : file not found
    [ISQL]ERROR: Could not SQLConnect

This means the ODBC driver library cannot be opened. The suggested cause "file not found"
may be misleading, however, as this message may be printed even if the file exists.
Troubleshooting:

*   Check whether the library exists at the specified location.
*   Check whether you have permission to *read* the library.
*   Check whether the library depends on other shared libraries that are not present:

    *    On Linux, use ``ldd /path/to/library.so``
    *    On OSX, use ``otool -L /path/to/library.dylib``

*   Check whether any superfluous non-printable characters are present in your ``odbc.ini``
    or ``odbcinst.ini`` near the ``Driver`` line. Been there, done that...


More subtle issues
------------------

There are still a few errors to make even when you can successfully establish a connection
with your database. Here are a few common ones:

*   *Unsuitable locale*: Some databases return data in a format dictated by your current
    locale settings. For example, unicode output may require a locale that supports
    UTF-8, such as ``en-US.utf-8``. Otherwise, replacement characters appear instead of
    unicode characters. Set the locale via environment variables such as ``LC_ALL``
    or check whether your driver supports setting a locale in its connection options.
*   *Time zones*: ODBC does not feature a dedicated type that is aware of time zones or
    the distinction between local time and UTC. Some databases, however, feature separate
    types for, e.g., timestamps with and without time zone information. ODBC drivers now
    need to find a way to make such information available to ODBC applications. A usual
    way to do this is to convert (some) values into the session time zone. This may lead
    to conflicting information when sessions with different time zones access the same
    database. The recommendation would be to fix the session time zone to UTC whenever possible
    to keep things consistent.
