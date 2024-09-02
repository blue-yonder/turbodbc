turbodbc_arrow = static_library(
  'turbodbc_arrow',
  [
    'Library/src/arrow_result_set.cpp',
    'Library/src/set_arrow_parameters.cpp',
  ],
  dependencies: [py.dependency(), pa_dep, arrow, arrow_python],
  include_directories: include_directories('Library', '../cpp_odbc/Library', '../turbodbc/Library'),
  link_with: [turbodbc]
)

if build_testing
  arrow_result_set_test = executable(
      'arrow_result_set_test',
      ['Test/tests/arrow_result_set_test.cpp'],
      dependencies: [py.dependency(embed: true), arrow, arrow_python, gtest, gtest_main, gmock],
      link_with: [turbodbc_arrow],
      include_directories: include_directories('Library', 'Test', '../cpp_odbc/Library', '../turbodbc/Library'),
    )
  test('arrow_result_set_test', arrow_result_set_test)
endif
