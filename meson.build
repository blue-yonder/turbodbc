project(
  'turbodbc', 'cpp',
  license: 'MIT',
  meson_version: '>= 1.0.0',
  default_options: [
    'buildtype=release',
    'cpp_std=c++17',
  ]
)

fs = import('fs')

cpp = meson.get_compiler('cpp')
add_global_arguments('-DBOOST_NO_CXX98_FUNCTION_BASE', language: 'cpp')
simdutf = dependency('simdutf')
arrow = dependency('arrow')
gtest = dependency('gtest')
gtest_main = dependency('gtest_main')
gmock = dependency('gmock')

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
numpy_nodepr_api = ['-DNPY_NO_DEPRECATED_API=NPY_1_9_API_VERSION']
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
if target_machine.system() != 'windows'
  unixodbc = cpp.find_library('odbc')
  deps += unixodbc
endif

cpp_odbc_sources = [
  'cpp/cpp_odbc/Library/src/level1/api.cpp',
  'cpp/cpp_odbc/Library/src/level1/level1.cpp',
  'cpp/cpp_odbc/Library/src/level1/unixodbc_backend.cpp',
  'cpp/cpp_odbc/Library/src/level1/unixodbc_backend_debug.cpp',
  'cpp/cpp_odbc/Library/src/level2/api.cpp',
  'cpp/cpp_odbc/Library/src/level2/handles.cpp',
  'cpp/cpp_odbc/Library/src/level2/input_string_buffer.cpp',
  'cpp/cpp_odbc/Library/src/level2/input_u16string_buffer.cpp',
  'cpp/cpp_odbc/Library/src/level2/level1_connector.cpp',
  'cpp/cpp_odbc/Library/src/level2/level2.cpp',
  'cpp/cpp_odbc/Library/src/level2/string_buffer.cpp',
  'cpp/cpp_odbc/Library/src/level2/u16string_buffer.cpp',
  'cpp/cpp_odbc/Library/src/level3/level3.cpp',
  'cpp/cpp_odbc/Library/src/level3/raii_connection.cpp',
  'cpp/cpp_odbc/Library/src/level3/raii_environment.cpp',
  'cpp/cpp_odbc/Library/src/level3/raii_statement.cpp',
  'cpp/cpp_odbc/Library/src/column_description.cpp',
  'cpp/cpp_odbc/Library/src/connection.cpp',
  'cpp/cpp_odbc/Library/src/cpp_odbc.cpp',
  'cpp/cpp_odbc/Library/src/environment.cpp',
  'cpp/cpp_odbc/Library/src/error.cpp',
  'cpp/cpp_odbc/Library/src/make_environment.cpp',
  'cpp/cpp_odbc/Library/src/multi_value_buffer.cpp',
  'cpp/cpp_odbc/Library/src/statement.cpp',
]

turbodbc_sources = cpp_odbc_sources + [
  'cpp/turbodbc/Library/src/buffer_size.cpp',
  'cpp/turbodbc/Library/src/column.cpp',
  'cpp/turbodbc/Library/src/command.cpp',
  'cpp/turbodbc/Library/src/configuration.cpp',
  'cpp/turbodbc/Library/src/connect.cpp',
  'cpp/turbodbc/Library/src/connection.cpp',
  'cpp/turbodbc/Library/src/cursor.cpp',
  'cpp/turbodbc/Library/src/description.cpp',
  'cpp/turbodbc/Library/src/descriptions/boolean_description.cpp',
  'cpp/turbodbc/Library/src/descriptions/date_description.cpp',
  'cpp/turbodbc/Library/src/descriptions/floating_point_description.cpp',
  'cpp/turbodbc/Library/src/descriptions/integer_description.cpp',
  'cpp/turbodbc/Library/src/descriptions/string_description.cpp',
  'cpp/turbodbc/Library/src/descriptions/timestamp_description.cpp',
  'cpp/turbodbc/Library/src/descriptions/unicode_description.cpp',
  'cpp/turbodbc/Library/src/errors.cpp',
  'cpp/turbodbc/Library/src/field_translator.cpp',
  'cpp/turbodbc/Library/src/field_translators/boolean_translator.cpp',
  'cpp/turbodbc/Library/src/field_translators/date_translator.cpp',
  'cpp/turbodbc/Library/src/field_translators/float64_translator.cpp',
  'cpp/turbodbc/Library/src/field_translators/int64_translator.cpp',
  'cpp/turbodbc/Library/src/field_translators/string_translator.cpp',
  'cpp/turbodbc/Library/src/field_translators/timestamp_translator.cpp',
  'cpp/turbodbc/Library/src/make_description.cpp',
  'cpp/turbodbc/Library/src/make_field_translator.cpp',
  'cpp/turbodbc/Library/src/parameter.cpp',
  'cpp/turbodbc/Library/src/parameter_sets/bound_parameter_set.cpp',
  'cpp/turbodbc/Library/src/parameter_sets/field_parameter_set.cpp',
  'cpp/turbodbc/Library/src/parameter_sets/set_field.cpp',
  'cpp/turbodbc/Library/src/result_sets/bound_result_set.cpp',
  'cpp/turbodbc/Library/src/result_sets/double_buffered_result_set.cpp',
  'cpp/turbodbc/Library/src/result_sets/field_result_set.cpp',
  'cpp/turbodbc/Library/src/result_sets/result_set.cpp',
  'cpp/turbodbc/Library/src/result_sets/row_based_result_set.cpp',
  'cpp/turbodbc/Library/src/string_helpers.cpp',
  'cpp/turbodbc/Library/src/time_helpers.cpp',
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
  'cpp/turbodbc_arrow/Library/src/arrow_result_set.cpp',
  'cpp/turbodbc_arrow/Library/src/set_arrow_parameters.cpp',
]

py.extension_module(
  'turbodbc_intern',
  turbodbc_sources,
  dependencies: deps,
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

cpp_odbc_test_sources = [
  'cpp/cpp_odbc/Test/cpp_odbc_test/level1_mock_api.cpp',
  'cpp/cpp_odbc/Test/cpp_odbc_test/level2_mock_api.cpp',
]

cpp_odbc_tests = [
  'column_description_test',
  'connection_test',
  'credentials_test',
  'environment_test',
  'error_test',
  'make_environment_test',
  'multi_value_buffer_test',
  'statement_test',
]
foreach t: cpp_odbc_tests
  e = executable(
    t,
    cpp_odbc_sources + cpp_odbc_test_sources + ['cpp/cpp_odbc/Test/tests/' + t + '.cpp'],
    dependencies: deps + [gtest, gtest_main, gmock],
    include_directories: include_directories('cpp/cpp_odbc/Library', 'cpp/cpp_odbc/Test'),
  )
  test(t, e)
endforeach
