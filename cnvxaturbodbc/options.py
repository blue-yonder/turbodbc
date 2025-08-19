from turbodbc_intern import Options


def make_options(
    read_buffer_size=None,
    parameter_sets_to_buffer=None,
    varchar_max_character_limit=None,
    prefer_unicode=None,
    use_async_io=None,
    autocommit=None,
    large_decimals_as_64_bit_types=None,
    limit_varchar_results_to_max=None,
    force_extra_capacity_for_unicode=None,
    fetch_wchar_as_char=None,
):
    """
    Create options that control how turbodbc interacts with a database. These
    options affect performance for the most part, but some options may require adjustment
    so that all features work correctly with certain databases.

    If a parameter is set to `None`, this means the default value is used.

    :param read_buffer_size: Affects performance. Controls the size of batches fetched from the
     database when reading result sets. Can be either an instance of ``turbodbc.Megabytes`` (recommended)
     or ``turbodbc.Rows``.
    :param parameter_sets_to_buffer: Affects performance. Number of parameter sets (rows) which shall be
     transferred to the server in a single batch when ``executemany()`` is called. Must be an integer.
    :param varchar_max_character_limit: Affects behavior/performance. If a result set contains fields
     of type ``VARCHAR(max)`` or ``NVARCHAR(max)`` or the equivalent type of your database, buffers
     will be allocated to hold the specified number of characters. This may lead to truncation. The
     default value is ``65535`` characters. Please note that large values reduce the risk of
     truncation, but may affect the number of rows in a batch of result sets (see ``read_buffer_size``).
     Please note that this option only relates to retrieving results, not sending parameters to the
     database.
    :param use_async_io: Affects performance. Set this option to ``True`` if you want to use asynchronous
     I/O, i.e., while Python is busy converting database results to Python objects, new result sets are
     fetched from the database in the background.
    :param prefer_unicode: May affect functionality and performance. Some databases do not support
     strings encoded with UTF-8, leading to UTF-8 characters being misinterpreted, misrepresented, or
     downright rejected. Set this option to ``True`` if you want to transfer character data using the
     UCS-2/UCS-16 encoding that use (multiple) two-byte instead of (multiple) one-byte characters.
    :param autocommit: Affects behavior. If set to ``True``, all queries and commands executed
     with ``cursor.execute()`` or ``cursor.executemany()`` will be succeeded by an implicit ``COMMIT``
     operation, persisting any changes made to the database. If not set or set to ``False``,
     users has to take care of calling ``cursor.commit()`` themselves.
    :param large_decimals_as_64_bit_types: Affects behavior. If set to ``True``, ``DECIMAL(x, y)``
     results with ``x > 18`` will be rendered as 64 bit integers (``y == 0``) or 64 bit floating
     point numbers (``y > 0``), respectively. Use this option if your decimal data types are larger
     than the data they actually hold. Using this data type can lead to overflow errors and loss
     of precision. If not set or set to ``False``, large decimals are rendered as strings.
    :param limit_varchar_results_to_max: Affects behavior/performance. If set to ``True``,
     any text-like fields such as ``VARCHAR(n)`` and ``NVARCHAR(n)`` will be limited to a maximum
     size of ``varchar_max_character_limit`` characters. This may lead to values being truncated,
     but reduces the amount of memory required to allocate string buffers, leading to larger, more
     efficient batches. If not set or set to ``False``, strings can exceed ``varchar_max_character_limit``
     in size if the database reports them this way. For fields such as ``TEXT``, some databases
     report a size of 2 billion characters.
     Please note that this option only relates to retrieving results, not sending parameters to the
     database.
    :param force_extra_capacity_for_unicode: Affects behavior/performance. Some ODBC drivers report the
     length of the ``VARCHAR``/``NVARCHAR`` field rather than the number of code points for which space is required
     to be allocated, resulting in string truncations. Set this option to ``True`` to increase the memory
     allocated for ``VARCHAR`` and ``NVARCHAR`` fields and prevent string truncations.
     Please note that this option only relates to retrieving results, not sending parameters to the
     database.
    :param fetch_wchar_as_char: Affects behavior. Some ODBC drivers retrieve single byte encoded strings
     into ``NVARCHAR`` fields of result sets, which are decoded incorrectly by turbodbc default settings,
     resulting in corrupt strings. Set this option to ``True`` to have turbodbc treat ``NVARCHAR`` types
     as narrow character types when decoding the fields in result sets.
     Please note that this option only relates to retrieving results, not sending parameters to the
     database.
    :return: An option struct that is suitable to pass to the ``turbodbc_options`` parameter of
     ``turbodbc.connect()``
    """
    options = Options()

    if read_buffer_size is not None:
        options.read_buffer_size = read_buffer_size

    if parameter_sets_to_buffer is not None:
        options.parameter_sets_to_buffer = parameter_sets_to_buffer

    if varchar_max_character_limit is not None:
        options.varchar_max_character_limit = varchar_max_character_limit

    if prefer_unicode is not None:
        options.prefer_unicode = prefer_unicode

    if use_async_io is not None:
        options.use_async_io = use_async_io

    if autocommit is not None:
        options.autocommit = autocommit

    if large_decimals_as_64_bit_types is not None:
        options.large_decimals_as_64_bit_types = large_decimals_as_64_bit_types

    if limit_varchar_results_to_max is not None:
        options.limit_varchar_results_to_max = limit_varchar_results_to_max

    if force_extra_capacity_for_unicode is not None:
        options.force_extra_capacity_for_unicode = force_extra_capacity_for_unicode

    if fetch_wchar_as_char is not None:
        options.fetch_wchar_as_char = fetch_wchar_as_char

    return options
