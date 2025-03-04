ODBC concepts
=============

.. _odbc_driver:

ODBC drivers
------------

ODBC drivers comply with the ODBC API, meaning that they offer a set of about
80 C functions with well-defined behavior that internally use database-specific
commands to achieve the desired behavior. There is some wiggle room that
allows ODBC drivers to implement certain things differently or even exclude support
for some advanced usage patterns. But in essence, all ODBC drivers are born more or
less equal.

ODBC drivers are easy to come by. Major database vendors offer ODBC drivers as free
downloads (`Microsoft SQL Server <https://www.microsoft.com/en-us/download/details.aspx?id=56567>`_,
`Exasol <https://www.exasol.com/portal/display/DOWNLOAD/6.0>`_,
`Teradata <https://downloads.teradata.com/download/connectivity/odbc-driver/windows>`_, etc).
Open source databases provide ODBC databases as part of their projects
(`PostgreSQL <https://odbc.postgresql.org>`_,
`Impala <https://www.cloudera.com/downloads/connectors/impala/odbc/2-5-37.html>`_,
`MongoDB <https://github.com/NYUITP/sp13_10g>`_).
Many ODBC drivers are also shipped with Linux distributions or are readily
available via `Homebrew <https://github.com/Homebrew/homebrew-core>`_ for OSX.
Last but not least, commercial ODBC drivers are available at
`Progress <https://www.progress.com/odbc>`_ or `easysoft <http://www.easysoft.com/index.html>`_,
claiming better performance than their freely available counterparts.


.. _odbc_driver_manager:

ODBC driver manager
-------------------

The driver manager is a somewhat odd centerpiece. It is a library that can be used
just like any ODBC driver. It provides definitions for various data types, and
actual ODBC drivers often rely on these definitions for compilation. The driver
manager has a built-in configuration of data sources. A data source has
a name (the data source name or DSN), is associated with an ODBC driver, contains
configuration options such as the database host or the connection locale, and sometimes
it also contains credentials for authentication with the database. Finally, the
driver manager typically comes with a tool to edit data sources.

Driver managers are less numerous, but still easily available on all major platforms.
Windows comes with a preinstalled ODBC database manager. On Linux and OSX, there
are competing driver managers in `unixodbc <http://www.unixodbc.org>`_ and
`iodbc <http://www.iodbc.org/dataspace/doc/iodbc/wiki/iodbcWiki/WelcomeVisitors>`_.

.. note::
    Turbodbc is tested with Windows's built-in driver manager and unixodbc on
    Linux and OSX.


.. _odbc_application:

ODBC applications
-----------------

Applications finally use the ODBC API and link to the driver manager. Any time they
open a connection, they need to specify the data source name that contains connection
attributes that relate to the desired database. Alternatively, they can specify all
necessary connection options directly.

Linking to the driver manager instead of the ODBC driver directly means that changing
to another driver is as simple as exchanging the connection string at runtime instead
of tediously linking to a new driver. Linking to the driver manager also means that the
driver manager handles many capability and compatibility options by transparently using
alternative functions and workarounds as required.