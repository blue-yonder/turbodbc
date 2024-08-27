import os
import sys

import pyarrow

# Ensure arrow_python${SHLIB_EXT} is on the PATH.
if sys.platform == "win32":
    for lib_dir in pyarrow.get_library_dirs():
        os.add_dll_directory(lib_dir)

from turbodbc_arrow_support import *  # noqa
