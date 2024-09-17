project(
  'turbodbc', 'cpp',
  license: 'MIT',
  meson_version: '>= 1.1.0',
  default_options: [
    'buildtype=release',
    'cpp_std=c++17',
    'warning_level=3',
    'build_testing=false',
  ]
)

fs = import('fs')

cpp = meson.get_compiler('cpp')
add_global_arguments('-DBOOST_NO_CXX98_FUNCTION_BASE', language: 'cpp')
add_global_arguments('-DGTEST_LINKED_AS_SHARED_LIBRARY=1', language: 'cpp')
if target_machine.system() == 'windows'
  add_global_arguments('-DNOMINMAX', language: 'cpp')
endif
simdutf = dependency('simdutf')
arrow = dependency('arrow')
build_testing = get_option('build_testing')
if build_testing
  gtest = dependency('gtest')
  gtest_main = dependency('gtest_main')
  gmock = dependency('gmock')
endif

# unixodbc = dependency('unixodbc')
py = import('python').find_installation()

# Taken from https://github.com/scipy/scipy/blob/e2ce1a57639dbfa5d1439cb542b67e3cbeda5d29/scipy/meson.build#L30
# The chdir is needed because within numpy there's an `import signal`
# statement, and we don't want that to pick up scipy's signal module rather
# than the stdlib module. The try-except is needed because when things are
# split across drives on Windows, there is no relative path and an exception
# gets raised. There may be other such cases, so add a catch-all and switch to
# an absolute path. Relative paths are needed when for example a virtualenv is
# placed inside the source tree; Meson rejects absolute paths to places inside
# the source tree.
# For cross-compilation it is often not possible to run the Python interpreter
# in order to retrieve numpy's include directory. It can be specified in the
# cross file instead:
#   [properties]
#   numpy-include-dir = /abspath/to/host-pythons/site-packages/numpy/core/include
#
# This uses the path as is, and avoids running the interpreter.
incdir_numpy = meson.get_external_property('numpy-include-dir', 'not-given')
if incdir_numpy == 'not-given'
  incdir_numpy = run_command(py,
    [
      '-c',
      '''import os
import numpy as np
try:
  incdir = os.path.relpath(np.get_include())
except Exception:
  incdir = np.get_include()
print(incdir)
  '''
    ],
    check: true
  ).stdout().strip()

  # We do need an absolute path to feed to `cc.find_library` below
  _incdir_numpy_abs = run_command(py,
    ['-c', 'import numpy; print(numpy.get_include())'],
    check: true
  ).stdout().strip()
else
  _incdir_numpy_abs = incdir_numpy
endif
inc_np = include_directories(incdir_numpy)
# Don't use the deprecated NumPy C API. Define this to a fixed version instead of
# NPY_API_VERSION in order not to break compilation for released SciPy versions
# when NumPy introduces a new deprecation.
numpy_nodepr_api = ['-DNPY_NO_DEPRECATED_API=NPY_1_21_API_VERSION']
np_dep = declare_dependency(include_directories: inc_np, compile_args: numpy_nodepr_api)

incdir_pyarrow = meson.get_external_property('pyarrow-include-dir', 'not-given')
if incdir_pyarrow == 'not-given'
  incdir_pyarrow = run_command(py,
    [
      '-c',
      '''import os
import pyarrow as pa
try:
  incdir = os.path.relpath(pa.get_include())
except Exception:
  incdir = pa.get_include()
print(incdir)
  '''
    ],
    check: true
  ).stdout().strip()

  # We do need an absolute path to feed to `cc.find_library` below
  _incdir_pyarrow_abs = run_command(py,
    ['-c', 'import pyarrow; print(pyarrow.get_include())'],
    check: true
  ).stdout().strip()
else
  _incdir_pyarrow_abs = incdir_pyarrow
endif
inc_pa = include_directories(incdir_pyarrow)
pa_dep = declare_dependency(include_directories: inc_pa)
arrow_python = cpp.find_library('arrow_python', dirs: [fs.parent(_incdir_pyarrow_abs)])

deps = [np_dep, pa_dep, arrow, arrow_python, simdutf]
if target_machine.system() == 'windows'
  odbc32 = cpp.find_library('odbc32')
  deps += odbc32
else
  unixodbc = cpp.find_library('odbc')
  deps += unixodbc
endif

subdir('cpp/cpp_odbc')
subdir('cpp/turbodbc')
subdir('cpp/turbodbc_arrow')

turbodbc_sources = [
  'cpp/turbodbc_python/Library/src/determine_parameter_type.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/buffer_size.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/column_info.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/connect.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/connection.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/cursor.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/error.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/module.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/options.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/python_parameter_set.cpp',
  'cpp/turbodbc_python/Library/src/python_bindings/python_result_set.cpp',
  'cpp/turbodbc_python/Library/src/python_parameter_set.cpp',
  'cpp/turbodbc_python/Library/src/python_result_set.cpp',
  'cpp/turbodbc_numpy/Library/src/binary_column.cpp',
  'cpp/turbodbc_numpy/Library/src/datetime_column.cpp',
  'cpp/turbodbc_numpy/Library/src/make_numpy_array.cpp',
  'cpp/turbodbc_numpy/Library/src/masked_column.cpp',
  'cpp/turbodbc_numpy/Library/src/numpy_result_set.cpp',
  'cpp/turbodbc_numpy/Library/src/numpy_type.cpp',
  'cpp/turbodbc_numpy/Library/src/set_numpy_parameters.cpp',
  'cpp/turbodbc_numpy/Library/src/string_column.cpp',
  'cpp/turbodbc_numpy/Library/src/unicode_column.cpp',
]

py.extension_module(
  'turbodbc_intern',
  turbodbc_sources,
  dependencies: deps,
  link_with: [cpp_odbc, turbodbc, turbodbc_arrow],
  include_directories: include_directories('cpp/cpp_odbc/Library', 'cpp/turbodbc/Library', 'cpp/turbodbc_python/Library', 'cpp/turbodbc_numpy/Library', 'cpp/turbodbc_arrow/Library'),
  install: true
)

py.install_sources(
  [
    'turbodbc/options.py',
    'turbodbc/connect.py',
    'turbodbc/__init__.py',
    'turbodbc/data_types.py',
    'turbodbc/api_constants.py',
    'turbodbc/connection.py',
    'turbodbc/arrow_support.py',
    'turbodbc/exceptions.py',
    'turbodbc/constructors.py',
    'turbodbc/cursor.py',
  ],
  subdir: 'turbodbc',
)

