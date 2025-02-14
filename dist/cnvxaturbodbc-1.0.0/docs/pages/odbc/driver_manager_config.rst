Driver manager configuration
============================

The driver manager needs to know to which databases to connect with which ODBC drivers.
This configuration needs to be maintained by the user.


Windows
-------

Windows comes with a preinstalled driver manager that can be configured with the
ODBC data source administrator. Please see Microsoft's
`official documentation <https://docs.microsoft.com/en-us/sql/odbc/admin/odbc-data-source-administrator>`_
for this. Besides adding your data sources, no special measures need to be done
for your configuration to be found.


Unixodbc (Linux and OSX)
------------------------

Unixodbc is a different beast. For one thing, you need to install it first.
That is usually an easy task involving a simple ``apt-get install unixodbc`` (Linux)
or ``brew install unixodbc`` (OSX with `Homebrew <https://github.com/Homebrew/homebrew-core>`_).

However, unixodbc can be configured in many ways, both with and without graphical guidance.
The official documentation is not always easy to follow, and finding what you are looking for
may be more difficult than you planned for and may involve looking into unixodbc's source code.

The following primer assumes that no graphic tools are used (as is often common in server environments).
It is not specific to turbodbc and based on information available at these locations:

*   `Unixodbc documentation "hub" <http://www.unixodbc.org/doc/>`_
*   `Details on using unixodbc without a GUI <http://www.unixodbc.org/odbcinst.html>`_
*   `Unixodbc man page <https://www.systutorials.com/docs/linux/man/7-unixODBC/>`_


ODBC configuration files
~~~~~~~~~~~~~~~~~~~~~~~~

Unixodbc's main configuration file is usually called ``odbc.ini``. ``odbc.ini`` defines
data sources that are available for connecting. It is a simple
`ini-style <https://en.wikipedia.org/wiki/INI_file>`_ text file with the following layout:

.. code-block:: ini

    [data source name]
    Driver = /path/to/driver_library.so
    Option1 = Value
    Option2 = Other value

    [other data source]
    Driver = Identifier specified in odbcinst.ini file
    OptionA = Value

The sections define data source names that can be used to connect with the respective
database. Each section requires a ``Driver`` key. The value of this key may either
contain the path to the database's :ref:`ODBC driver <odbc_driver>` or a key that
identifies the driver in unixodbc's other configuration file ``odbcinst.ini``. Each section
may contain an arbitrary number of key-value pairs that specify further connection
options. These connection options are driver-specific, so you need to refer to the
ODBC driver's reference for that.

As mentioned before, unixodbc features a second (and optional) configuration file
usually called ``odbcinst.ini``. This file lists available ODBC drivers and labels
them for convenient reference in ``odbc.ini``. The file also follows the
`ini-style <https://en.wikipedia.org/wiki/INI_file>`_ convention:

.. code-block:: ini

    [driver A]
    Driver = /path/to/driver_library.so
    Threading = 2
    Description = A driver to access ShinyDB databases

    [driver B]
    Driver = /some/other/driver/library.so

The sections define names that can be used as values for the ``Driver`` keys in
``odbc.ini``. Each section needs to feature ``Driver`` keys themselves, where
the values represent paths to the respective ODBC drivers. Some additional
options are available such as the ``Threading`` level (see
`unixodbc's source code <https://sourceforge.net/p/unixodbc/code/HEAD/tree/trunk/DriverManager/__handles.c#l260>`_
for details) or a ``Description`` field.


Configuration file placement options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Unixodbc has a few places where it looks for its configuration files:

*   Global configuration files are found in ``/etc/odbc.ini`` and ``/etc/odbcinst.ini``.
    Data sources defined in ``/etc/odbc.ini`` are available to all users of your computer.
    Drivers defined in ``/etc/odbcinst.ini`` can be used by all users of your computer.
*   Users can define additional data sources by adding the file ``~/.odbc.ini`` to
    their home directory. It seems that a file called ``~/.odbcinst.ini`` has no effect.
*   Users can add a folder in which to look for configuration files by setting the
    ``ODBCSYSINI`` environment variable:

    ::

        > export ODBCSYSINI=/my/folder

    This will override the configuration files found at ``/etc``. Place your configuration
    files at ``/my/folder/odbc.ini`` and ``/my/folder/odbcinst.ini``.
*   Users can override the path for the user-specific ``odbc.ini`` file by setting the
    ``ODBCINI`` environment variable:

    ::

        > export ODBCINI=/full/path/to/odbc.ini

    If you set this option, unixodbc will no longer consider ``~/.odbc.ini``.

    .. note::
        Do not expect the ``ODBCINSTINI`` environment variable to work just as ``ODBCINI``.
        Instead, the ``ODBCINSTINI`` specifies the file name of ``odbcinst.ini`` relative
        to the value of the ``ODBCSYSINI`` variable. I suggest not to use this variable
        since it is outright confusing.


Configuration file placement recommendations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here are a few typical scenarios:

*   *First steps with unixodbc*: Create a new folder that contains ``odbc.ini`` and
    ``odbcinst.ini``. Set the ``ODBCSYSINI`` variable to this folder.
*   *Experimenting with a new database/driver*: Create a new folder that contains ``odbc.ini`` and
    ``odbcinst.ini``. Set the ``ODBCSYSINI`` variable to this folder.
*   *Provision a system with drivers*: Place an ``odbcinst.ini`` file at ``/etc/odbcinst.ini``.
    Tell users to configure their databases using ``~/odbc.ini`` or setting ``ODBCINI``.
*   *Switching between multiple distinct configurations (test/production)*: Use the ``ODBCSYSINI`` variable
    if the configurations do not share common drivers. Otherwise, use the ``ODBCINI`` variable
    to switch between different ``odbc.ini`` files.
