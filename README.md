![turbodbc logo](/page/logo.png?raw=true "turbodbc logo")

Turbodbc - Turbocharged database access for data scientists.
============================================================

[![Build Status](https://github.com/blue-yonder/turbodbc/workflows/CI/badge.svg?branch=master)](https://github.com/blue-yonder/turbodbc/actions)
[![Documentation Status](https://readthedocs.org/projects/turbodbc/badge/?version=latest)](http://turbodbc.readthedocs.io/en/latest/?badge=latest)
[![codecov](https://codecov.io/gh/blue-yonder/turbodbc/branch/master/graph/badge.svg)](https://codecov.io/gh/blue-yonder/turbodbc)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/turbodbc/badges/installer/conda.svg)](https://conda.anaconda.org/conda-forge)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/turbodbc/badges/downloads.svg)](https://anaconda.org/conda-forge/turbodbc)

Turbodbc is a Python module to access relational databases via the
[Open Database Connectivity (ODBC)](https://en.wikipedia.org/wiki/Open_Database_Connectivity)
interface. Its primary target audience are data scientists
that use databases for which no efficient native Python drivers are available.

For maximum compatibility, turbodbc complies with the
[Python Database API Specification 2.0 (PEP 249)](https://www.python.org/dev/peps/pep-0249/).
For maximum performance, turbodbc offers built-in [NumPy](http://www.numpy.org) and
[Apache Arrow](https://arrow.apache.org) support
and internally relies on batched data transfer instead of single-record communication as
other popular ODBC modules do.

Turbodbc is free to use ([MIT license](https://github.com/blue-yonder/turbodbc/blob/master/LICENSE)),
open source ([GitHub](https://github.com/blue-yonder/turbodbc)),
works with Python 3.9+, and is available for Linux, macOS, and Windows.

Turbodbc is routinely tested with [MySQL](https://www.mysql.com),
[PostgreSQL](https://www.postgresql.org), [EXASOL](http://www.exasol.com),
and [MSSQL](http://microsoft.com/sql), but probably also works with your database.


Nice! Where can I find documentation?
-------------------------------------

Follow this link to the [latest turbodbc documentation](http://turbodbc.readthedocs.io/en/latest/).
The documentation explains how to install and use turbodbc, and also provides
answers to many questions you might have.

Is turbodbc on Twitter?
-----------------------

Yes, it is! Just follow [@turbodbc](https://twitter.com/turbodbc)
for the latest turbodbc talk and news about related technologies.
