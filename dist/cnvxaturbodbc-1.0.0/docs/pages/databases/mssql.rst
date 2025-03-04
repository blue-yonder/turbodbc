Microsoft SQL Server (MSSQL)
============================

`Microsoft SQL Server <https://www.microsoft.com/sql>`_ (MSSQL) is part of turbodbc's
integration databases. That means that each commit in turbodbc's repository
is automatically tested against MSSQL to ensure compatibility.
Here are the recommended settings for connecting to a Microsoft SQL database via ODBC
using the turbodbc module for Python.


Recommended odbcinst.ini (Linux)
--------------------------------

On Linux, you have the choice between two popular drivers.

Official Microsoft ODBC driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Microsoft offers an `official ODBC driver <https://docs.microsoft.com/en-us/sql/connect/odbc/linux/microsoft-odbc-driver-for-sql-server-on-linux>`_
for selected `modern Linux distributions <https://docs.microsoft.com/en-us/sql/connect/odbc/linux/installing-the-microsoft-odbc-driver-for-sql-server-on-linux>`_.

.. code-block:: ini

    [MSSQL Driver]
    Driver=/opt/microsoft/msodbcsql/lib64/libmsodbcsql-13.1.so.4.0


FreeTDS
~~~~~~~

`FreeTDS <http://www.freetds.org>`_ is an `open source <https://github.com/FreeTDS/freetds>`_
ODBC driver that supports MSSQL. It is stable, has been around for well over decade and is actively
maintained. However, it is not officially supported by Microsoft.

.. code-block:: ini

    [FreeTDS Driver]
    Driver = /usr/local/lib/libtdsodbc.so



Recommended odbcinst.ini (OSX)
------------------------------

`FreeTDS <http://www.freetds.org>`_ seems to be the only available driver for OSX
that can connect to MSSQL databases.


.. code-block:: ini

    [FreeTDS Driver]
    Driver = /usr/local/lib/libtdsodbc.so


Recommended data source configuration
-------------------------------------

Official Microsoft ODBC driver (Windows)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Put these values in your registry under the given key. Be sure to prefer the
`latest ODBC driver <https://www.microsoft.com/en-us/download/details.aspx?id=56567>`_
over any driver that may come bundled with your Windows version.

.. code-block:: ini

    [HKEY_LOCAL_MACHINE\SOFTWARE\ODBC\ODBC.INI\MSSQL]
    "Driver"="C:\\Windows\\system32\\msodbcsql17.dll"
    "Server"="<host>"
    "Database"="<database>"

Official Microsoft ODBC driver (Linux)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: ini

    [MSSQL]
    Driver         = MSSQL Driver
    Server         = <host>,<port>
    Database       = <database>

.. note::
    You cannot specify credentials for MSSQL databases in ``odbc.ini``.

FreeTDS data sources
~~~~~~~~~~~~~~~~~~~~

.. code-block:: ini

    [MSSQL]
    Driver   = FreeTDS Driver
    Server   = <host>
    Port     = <post>
    Database = <database>

.. note::
    You cannot specify credentials for MSSQL databases in ``odbc.ini``.


Recommended turbodbc configuration
----------------------------------

The default turbodbc connection options have issues with Unicode strings
on MSSQL. Please make sure to set the ``prefer_unicode``
:ref:`option <advanced_usage_options_prefer_unicode>`.

::

    >>> from turbodbc import connect, make_options
    >>> options = make_options(prefer_unicode=True)
    >>> connect(dsn="MSSQL", turbodbc_options=options)

.. warning::
    If you forget to set ``prefer_unicode``, you may get anything from
    garbled up characters (e.g., ``u'\xe2\x99\xa5'`` instead of the unicode
    character ``u'\u2665'``) or even ODBC error messages such as
    ``[FreeTDS][SQL Server]Invalid cursor state``.
