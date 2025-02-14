Turbodbc - Turbocharged database access for data scientists
===========================================================

.. image::
    ../page/logo.svg
    :target: https://github.com/blue-yonder/turbodbc

Turbodbc is a Python module to access relational databases via the
`Open Database Connectivity (ODBC) <https://en.wikipedia.org/wiki/Open_Database_Connectivity>`_
interface. Its primary target audience are data scientist
that use databases for which no efficient native Python drivers are available.

For maximum compatibility, turbodbc complies with the
`Python Database API Specification 2.0 (PEP 249) <https://www.python.org/dev/peps/pep-0249/>`_.
For maximum performance, turbodbc offers built-in `NumPy <http://www.numpy.org>`_ and
`Apache Arrow <https://arrow.apache.org>`_ support
and internally relies on batched data transfer instead of single-record communication as
other popular ODBC modules do.

Turbodbc is free to use (`MIT license <https://github.com/blue-yonder/turbodbc/blob/master/LICENSE>`_),
open source (`GitHub <https://github.com/blue-yonder/turbodbc>`_),
works with Python 3.6+, and is available for Linux, macOS, and Windows.

Turbodbc is routinely tested with `MySQL <https://www.mysql.com>`_,
`PostgreSQL <https://www.postgresql.org>`_, `EXASOL <http://www.exasol.com>`_,
and `MSSQL <http://microsoft.com/sql>`_, but probably also works with your database.


.. toctree::
    :maxdepth: 1

    pages/introduction
    pages/getting_started
    pages/advanced_usage
    pages/odbc_configuration
    pages/databases
    pages/changelog
    pages/troubleshooting
    pages/faq
    pages/contributing
    pages/api_reference


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
