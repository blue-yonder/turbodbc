Netezza
=======

Although `IBM Netezza <https://www.ibm.com/analytics/netezza>`_  is not an integration tested
database for turbodbc, some features have been added to support working with Netezza.
Here are the recommended settings for connecting to a Netezza database via ODBC
using the turbodbc module for Python.

.. note::
    Probably due to the prefetch buffering, turbodbc seems to be about as fast as using pyodbc,
    however, fetching using NumPy or Arrow calls (``cursor.fetchallnumpy()`` or ``cursor.fetchallarrow()``)
    is approximately 3 times faster due to avoiding the conversion from SQL types to Python
    types to NumPy/Arrow types and just converting directly to NumPy/Arrow types.


Recommended odbcinst.ini (Linux)
--------------------------------

.. code-block:: ini

    [NZSQL]
    Driver                     = /path/to/nz/lib/libnzsqlodbc3.so  # 32bit driver
    Driver64                   = /path/to/nz/lib64/libnzodbc.so    # 64bit driver
    UnicodeTranslationOption   = utf8
    CharacterTranslationOption = all
    PreFetch                   = 10000
    Socket                     = 32000


Recommended data source configuration
-------------------------------------

.. code-block:: ini

    [Netezza]
    Driver                     = NZSQL
    Description                = NetezzaSQL ODBC Connection
    Servername                 = <server_hostname>
    Port                       = 5480
    Database                   = <default_database>
    Username                   = <username>
    Password                   = <password>


Recommended turbodbc configuration
----------------------------------

The default turbodbc connection options works for Netezza. However, ``NVARCHAR``
fields will be corrupted with the default settings. If you have ``NVARCHAR`` fields
that you need to retrieve correctly you can set a few turbodbc options to support
this, but this will cause turbodbc to use a lot more memory when retrieving result
sets containing either ``VARCHAR`` or ``NVARCHAR`` fields (4x more per ``VARCHAR``
field and 2x more per ``NVARCHAR`` field).

See the :ref:`advanced options <advanced_usage_options>` for details.

::

    >>> from turbodbc import connect, make_options, Megabytes
    >>> options = make_options(read_buffer_size=Megabytes(100))
    >>> connect(dsn="Netezza", turbodbc_options=options)

If retrieving string heavy datasets containing ``NVARCHAR`` fields:

::

    >>> from turbodbc import connect, make_options, Megabytes
    >>> options = make_options(read_buffer_size=Megabytes(250), fetch_wchar_as_char=True, force_extra_capacity_for_unicode=True)
    >>> connect(dsn="Netezza", turbodbc_options=options)
