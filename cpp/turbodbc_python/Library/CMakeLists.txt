pybind11_add_module(turbodbc_python
  src/determine_parameter_type.cpp
  src/python_bindings/buffer_size.cpp
  src/python_bindings/column_info.cpp
  src/python_bindings/connect.cpp
  src/python_bindings/connection.cpp
  src/python_bindings/cursor.cpp
  src/python_bindings/error.cpp
  src/python_bindings/module.cpp
  src/python_bindings/options.cpp
  src/python_bindings/python_parameter_set.cpp
  src/python_bindings/python_result_set.cpp
  src/python_parameter_set.cpp
  src/python_result_set.cpp
)

set_target_properties(turbodbc_python PROPERTIES 
    PREFIX ""
    OUTPUT_NAME turbodbc_intern
    INSTALL_RPATH "../../.."
)

target_link_libraries(turbodbc_python
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC ${Odbc_LIBRARIES}
    PUBLIC cpp_odbc
    PUBLIC turbodbc
)

add_custom_command(TARGET turbodbc_python POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE:turbodbc_python> ${CMAKE_BINARY_DIR})

if (APPLE)
   set_target_properties(turbodbc_python PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

install(
    TARGETS turbodbc_python
    DESTINATION lib
)

install(
    DIRECTORY turbodbc_python
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".git" EXCLUDE
)

install(
    DIRECTORY src/
    DESTINATION src/turbodbc_python
    FILES_MATCHING PATTERN "*.cpp"
    PATTERN ".git" EXCLUDE
)
