MySQL
=====

`MySQL <https://www.mysql.com>`_ is part of turbodbc's integration databases.
That means that each commit in turbodbc's repository
is automatically tested against MySQL to ensure compatibility.
Here are the recommended settings for connecting to a MySQL database via ODBC
using the turbodbc module for Python.

.. note::
    You can use the MySQL ODBC driver to connect with databases that use the
    MySQL wire protocol. Examples for such databases are
    `MariaDB <https://mariadb.org>`_,
    `Amazon Aurora DB <https://aws.amazon.com/rds/aurora/details/?nc1=h_ls>`_, or
    `MemSQL <http://www.memsql.com>`_.


Recommended odbcinst.ini (Linux)
--------------------------------

.. code-block:: ini

    [MySQL Driver]
    Driver      = /usr/lib/x86_64-linux-gnu/odbc/libmyodbc.so
    Threading   = 2

*   ``Threading = 2`` is a safe choice to avoid potential thread issues with the driver,
    but you can also attempt using the driver without this option.


Recommended data source configuration
-------------------------------------

.. code-block:: ini

    [MySQL]
    Driver   = MySQL Driver
    SERVER   = <host>
    UID      = <user>
    PASSWORD = <password>
    DATABASE = <database name>
    PORT     = <port, default is 3306>
    INITSTMT = set session time_zone ='+00:00';

*   ``INITSTMT = set session time_zone ='+00:00';`` sets the session time zone to
    UTC. This will yield `consistent values <https://dev.mysql.com/doc/refman/5.7/en/datetime.html>`_
    for fields of type ``TIMESTAMP``.


Recommended turbodbc configuration
----------------------------------

The default turbodbc connection options work fine for MySQL.

::

    >>> from turbodbc import connect
    >>> connect(dsn="MySQL")
