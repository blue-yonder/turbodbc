Frequently asked questions
==========================


Can I use turbodbc together with SQLAlchemy?
--------------------------------------------

Using Turbodbc in combination with SQLAlchemy is possible for a (currently) limited number of databases:

*   `EXASOL <http://www.exasol.com>`_: `sqlalchemy_exasol <https://github.com/blue-yonder/sqlalchemy_exasol>`_
*   `MSSQL <http://microsoft.com/sql>`_: `sqlalchemy-turbodbc <https://github.com/dirkjonker/sqlalchemy-turbodbc>`_

All of the above packages are available on PyPI. There are also more experimental implementations
available:

*   `Vertica <https://www.vertica.com>`_: Unofficial
    `fork of sqlalchemy-vertica <https://github.com/startappdev/sqlalchemy-vertica>`_

Where would I report issues concerning turbodbc?
------------------------------------------------

In this case, please use turbodbc's issue tracker on `GitHub`_.


Where can I ask questions regarding turbodbc?
---------------------------------------------

Basically, you can ask them anywhere, chances to get a helpful answer may vary, though.
I suggest asking questions either using turbodbc's issue tracker on
`GitHub`_ or by heading over to
`stackoverflow <http://stackoverflow.com/search?q=turbodbc>`_.


Is there a guided tour through turbodbc's entrails?
---------------------------------------------------

Yes, there is! Check out these blog posts on the making of turbodbc:

*   Part one: `Wrestling with the side effects of a C API <https://tech.jda.com/making-of-turbodbc-part-1-wrestling-with-the-side-effects-of-a-c-api/>`_.
    This explains the C++ layer that is used to handle all calls to the ODBC API.
*   Part two: `C++ to Python <https://tech.jda.com/making-of-turbodbc-part-2-c-to-python/>`_
    This explains how concepts of the ODBC API are transformed into an API compliant
    with Python's database API, including making use of `pybind11 <https://github.com/pybind/pybind11>`_.


I love Twitter! Is turbodbc on Twitter?
---------------------------------------

Yes, it is! Just follow `@turbodbc <https://twitter.com/turbodbc>`_
for the latest turbodbc talk and news about related technologies.


How can I find our more about turbodbc's latest developments?
-------------------------------------------------------------

There are a few options:

*   Watch the turbodbc project on `GitHub`_. This way, you will get mails for new issues,
    updates issues, and the like.
*   Periodically read turbodbc's
    `change log <https://github.com/blue-yonder/turbodbc/blob/master/CHANGELOG.rst>`_
*   Follow `@turbodbc <https://twitter.com/turbodbc>`_ on Twitter. There will be tweets
    for new releases.


.. _GitHub: https://github.com/blue-yonder/turbodbc
