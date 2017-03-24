from turbodbc_intern import Options

def make_options(read_buffer_size=None,
                 parameter_sets_to_buffer=None,
                 prefer_unicode=None,
                 use_async_io=None):
    """
    Create options that control how turbodbc interacts with a database. These
    options affect performance for the most part, but some options may require adjustment
    so that all features work correctly with certain databases.

    If a parameter is set to `None`, this means the default value is used.

    :param read_buffer_size: Affects performance. Controls the size of batches fetched from the
     database when reading result sets. Can be either an instance of `turbodbc.Megabytes` (recommended)
     or `turbodbc.Rows`.
    :param parameter_sets_to_buffer: Affects performance. Number of parameter sets (rows) which shall be
     transferred to the server in a single batch when executemany() is called. Must be an integer.
    :param use_async_io: Affects performance. Set this option to `True` if you want to use asynchronous
     I/O, i.e., while Python is busy converting database results to Python objects, new result sets are
     fetched from the database in the background.
    :param prefer_unicode: May affect functionality and performance. Some databases do not support
     strings encoded with UTF-8, leading to UTF-8 characters being misinterpreted, misrepresented, or
     downright rejected. Set this option to `True` if you want to transfer character data using the
     UCS-2/UCS-16 encoding that use (multiple) two-byte instead of (multiple) one-byte characters.
    :return: An option struct that is suitable to pass to the `turbodbc_options` parameter of
     `turbodbc.connect()`
    """
    options = Options()

    if not read_buffer_size is None:
        options.read_buffer_size = read_buffer_size

    if not parameter_sets_to_buffer is None:
        options.parameter_sets_to_buffer = parameter_sets_to_buffer

    if not prefer_unicode is None:
        options.prefer_unicode = prefer_unicode

    if not use_async_io is None:
        options.use_async_io = use_async_io

    return options
