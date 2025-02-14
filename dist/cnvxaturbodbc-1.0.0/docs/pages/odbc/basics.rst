ODBC basics
===========

ODBC is the abbreviation for `open database connectivity <https://en.wikipedia.org/wiki/Open_Database_Connectivity>`_,
a standard for interacting with relational databases that has been considerably
influenced by Microsoft. The aim of the standard is that applications can work
with multiple databases with little to no adjustments in code.

This is made possible by combining three components with each other:

*   Database vendors supply :ref:`ODBC drivers <odbc_driver>`.
*   An ODBC :ref:`driver manager <odbc_driver_manager>` manages ODBC data sources.
*   :ref:`Applications <odbc_application>` use the ODBC driver manager to connect to data sources.

Turbodbc makes it easy to build applications that use the ODBC driver manager,
but it still requires the driver manager to be configured correctly so that your
databases are found.