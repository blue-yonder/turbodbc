if (WIN32)
    include (GenerateExportHeader)
endif()

add_library(cpp_odbc
  src/column_description.cpp
  src/connection.cpp
  src/cpp_odbc.cpp
  src/environment.cpp
  src/error.cpp
  src/make_environment.cpp
  src/multi_value_buffer.cpp
  src/statement.cpp
  src/level1/api.cpp
  src/level1/level1.cpp
  src/level1/unixodbc_backend.cpp
  src/level1/unixodbc_backend_debug.cpp
  src/level2/api.cpp
  src/level2/handles.cpp
  src/level2/input_string_buffer.cpp
  src/level2/input_u16string_buffer.cpp
  src/level2/level1_connector.cpp
  src/level2/level2.cpp
  src/level2/string_buffer.cpp
  src/level2/u16string_buffer.cpp
  src/level3/level3.cpp
  src/level3/raii_connection.cpp
  src/level3/raii_environment.cpp
  src/level3/raii_statement.cpp)

target_link_libraries(cpp_odbc
    ${Boost_LIBRARIES}
    PUBLIC ${Simdutf_LIBRARIES}
    ${Odbc_LIBRARIES}
)

IF(WIN32)
add_custom_command(TARGET cpp_odbc POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE:cpp_odbc> ${CMAKE_BINARY_DIR})
ENDIF()

install(
    TARGETS cpp_odbc
    DESTINATION lib
)

install(
    DIRECTORY cpp_odbc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".git" EXCLUDE
)

install(
    DIRECTORY src/
    DESTINATION src/cpp_odbc
    FILES_MATCHING PATTERN "*.cpp"
    PATTERN ".git" EXCLUDE
)

file(GLOB USE_SCRIPTS "use_*.sh")
install(
    FILES ${USE_SCRIPTS}
    DESTINATION .
)
