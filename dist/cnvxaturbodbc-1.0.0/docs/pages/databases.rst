.. _database_configuration:

Databases configuration and performance
=======================================

As already outlined in the more general :ref:`ODBC configuration <odbc_configuration>`
section, connecting with your database via ODBC can be a real pain. Making matters worse,
database performance may significantly depend on the configuration as well.

Well, this section tries to make life just a tad easier by providing recommended
configurations for various databases. For some databases, comparisons with other
database access modules are provided as well so that you know what kind of
performance to expect.

.. note::
    The quality of the :ref:`ODBC driver <odbc_driver>` for a given database heavily
    affects performance of all ODBC applications using this driver. Even though
    turbodbc was built to exploit buffering and what else the ODBC API has to offer,
    it cannot work wonders when the ODBC driver is not up to the task. In such circumstances,
    other, non-ODBC technologies may be available that outperform turbodbc
    by a considerable margin.


.. toctree::
    :maxdepth: 1

    databases/exasol
    databases/postgresql
    databases/mysql
    databases/mssql
    databases/netezza
