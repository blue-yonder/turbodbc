Overview
--------

cpp_odbc is a C++ library which provides high-level C++ classes and functions to
connect with ODBC databases. In contrast to the unixODBC library, cpp_odbc
handles buffer management, throws exceptions in case of errors, and generally
avoids pointers.

cpp_odbc is not designed to be an actual driver or to implement a more general
database interface. It is intended as the basis to build such drivers, though.

