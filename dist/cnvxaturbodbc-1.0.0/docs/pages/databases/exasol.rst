Exasol
======

`Exasol <http://www.exasol.com>`_ is one of turbodbc's main development
databases, and also provided the initial motivation for creating turbodbc.
Here are the recommended settings for connecting to an Exasol database via ODBC
using the turbodbc module for Python.


Recommended odbcinst.ini (Linux)
--------------------------------

.. code-block:: ini

    [Exasol driver]
    Driver = /path/to/libexaodbc-uo2214lv1.so    # only when libodbc.so.2 is not present
    Driver = /path/to/libexaodbc-uo2214lv2.so    # only when libodbc.so.2 is present
    Threading = 2

*   Exasol ships drivers for various versions of unixodbc. Any modern system should use the
    ``uo2214`` driver variants. Choose the ``lv1`` version if your system contains the file
    ``libodbc.so.1``. If it does not, choose ``lv2`` instead.
*   ``Threading = 2`` seems to be required to handle some thread issues with the driver.


Recommended odbcinst.ini (OSX)
------------------------------

.. code-block:: ini

    [Exasol driver]
    Driver = /path/to/libexaodbc-io418sys.dylib
    Threading = 2

*   The driver listed here is built with the iodbc library. All turbodbc tests work
    with this driver even though turbodbc uses unixodbc.
*   ``Threading = 2`` seems to be required to handle some thread issues with the driver.


Recommended data source configuration
-------------------------------------

.. code-block:: ini

    [Exasol]
    DRIVER = Exasol driver
    EXAHOST = <host>:<port_range>
    EXAUID = <user>
    EXAPWD = <password>
    EXASCHEMA = <default_schema>
    CONNECTIONLCALL = en_US.utf-8

*   ``CONNECTIONLCALL`` is set to a locale with unicode support to avoid problems with
    retrieving Unicode characters.


Recommended turbodbc configuration
----------------------------------

The default turbodbc connection options work fine for Exasol. You can probably
tune the performance a little by increasing the read buffer size to 100 Megabytes.
Exasol claims that their database works best with this setting.

See the :ref:`advanced options <advanced_usage_options>` for details.

::

    >>> from turbodbc import connect, make_options, Megabytes
    >>> options = make_options(read_buffer_size=Megabytes(100))
    >>> connect(dsn="Exasol", turbodbc_options=options)
