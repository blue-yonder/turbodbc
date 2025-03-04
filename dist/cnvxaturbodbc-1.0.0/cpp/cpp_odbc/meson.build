cpp_odbc_sources = [
  'Library/src/level1/api.cpp',
  'Library/src/level1/level1.cpp',
  'Library/src/level1/unixodbc_backend.cpp',
  'Library/src/level1/unixodbc_backend_debug.cpp',
  'Library/src/level2/api.cpp',
  'Library/src/level2/handles.cpp',
  'Library/src/level2/input_string_buffer.cpp',
  'Library/src/level2/input_u16string_buffer.cpp',
  'Library/src/level2/level1_connector.cpp',
  'Library/src/level2/level2.cpp',
  'Library/src/level2/string_buffer.cpp',
  'Library/src/level2/u16string_buffer.cpp',
  'Library/src/level3/level3.cpp',
  'Library/src/level3/raii_connection.cpp',
  'Library/src/level3/raii_environment.cpp',
  'Library/src/level3/raii_statement.cpp',
  'Library/src/column_description.cpp',
  'Library/src/connection.cpp',
  'Library/src/cpp_odbc.cpp',
  'Library/src/environment.cpp',
  'Library/src/error.cpp',
  'Library/src/make_environment.cpp',
  'Library/src/multi_value_buffer.cpp',
  'Library/src/statement.cpp',
]
cpp_odbc = static_library(
  'cpp_odbc',
  cpp_odbc_sources,
  dependencies: [simdutf],
  include_directories: include_directories('Library'),
)

if build_testing
  cpp_odbc_test_sources = [
    'Test/cpp_odbc_test/level1_mock_api.cpp',
    'Test/cpp_odbc_test/level2_mock_api.cpp',
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
      cpp_odbc_test_sources + ['Test/tests/' + t + '.cpp'],
      dependencies: deps + [gtest, gtest_main, gmock],
      link_with: [cpp_odbc],
      include_directories: include_directories('Library', 'Test'),
    )
    test(t, e)
  endforeach
endif
