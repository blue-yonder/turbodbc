include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SYSTEM $ENV{GOOGLETEST_INCLUDE_DIR})
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/pybind11/include")


file(GLOB_RECURSE TEST_FILES "*.cpp")

link_directories($ENV{GOOGLETEST_LIB_DIR})

add_executable(turbodbc_arrow_test ${TEST_FILES})
add_dependencies(turbodbc_arrow_test googletest_build)

add_dependencies(turbodbc_arrow_test
    refresh_cmake_configuration
)

target_link_libraries(turbodbc_arrow_test
    turbodbc
    turbodbc_arrow_support
    cpp_odbc
    ${Odbc_LIBRARIES}
    ${PYTHON_LIBRARY}
    gmock_main
    gtest
    gmock
)

if (MSVC)
    target_link_libraries(turbodbc_arrow_test
        ${ARROW_SHARED_IMP_LIB}
        ${ARROW_PYTHON_SHARED_IMP_LIB}
    )
endif()

if (UNIX)
    target_link_libraries(turbodbc_arrow_test
        pthread
    )
endif()

add_test(NAME turbodbc_arrow_unit_test
         COMMAND turbodbc_arrow_test --gtest_output=xml:${CMAKE_BINARY_DIR}/turbodbc_arrow_test.xml)

if (WIN32)
    get_filename_component(ARROW_LIBS "${ARROW_SHARED_LIB}" DIRECTORY)
    string(REPLACE ";" "\\;" MASKED_PATH "$ENV{PATH}")
    set_tests_properties(
        turbodbc_arrow_unit_test
        PROPERTIES
        ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}\;${ARROW_LIBS}\;${MASKED_PATH}"
    )
endif()
