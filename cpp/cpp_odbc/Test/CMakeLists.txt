include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SYSTEM $ENV{GOOGLETEST_INCLUDE_DIR})

file(GLOB_RECURSE TEST_FILES "*.cpp")

link_directories($ENV{GOOGLETEST_LIB_DIR})

add_executable(cpp_odbc_test ${TEST_FILES})
add_dependencies(cpp_odbc_test googletest_build)

add_dependencies(cpp_odbc_test
    refresh_cmake_configuration
)

target_link_libraries(cpp_odbc_test
    ${GMOCK_MAIN_LIBRARY}
    ${GMOCK_LIBRARY}
    ${GTEST_LIBRARY}
	cpp_odbc
    ${Odbc_LIBRARIES}
)

IF(UNIX)
    target_link_libraries(cpp_odbc_test
        pthread
    )
ENDIF()

add_test(NAME cpp_odbc_unit_test
         COMMAND cpp_odbc_test --gtest_output=xml:${CMAKE_BINARY_DIR}/cpp_odbc_test.xml)

IF(WIN32)
    string(REPLACE ";" "\\;" MASKED_PATH "$ENV{PATH}")
    set_tests_properties(
        cpp_odbc_unit_test
        PROPERTIES
        ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}\;${MASKED_PATH}"
    )
ENDIF()
