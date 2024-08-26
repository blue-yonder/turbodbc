pybind11_add_module(turbodbc_numpy_support
  src/binary_column.cpp
  src/datetime_column.cpp
  src/make_numpy_array.cpp
  src/masked_column.cpp
  src/numpy_result_set.cpp
  src/numpy_type.cpp
  src/python_bindings.cpp
  src/set_numpy_parameters.cpp
  src/string_column.cpp
  src/unicode_column.cpp
)

set_target_properties(turbodbc_numpy_support PROPERTIES 
    PREFIX ""
    INSTALL_RPATH "../../.."
)

target_link_libraries(turbodbc_numpy_support
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC ${Odbc_LIBRARIES}
    PUBLIC cpp_odbc
    PUBLIC turbodbc
)

add_custom_command(TARGET turbodbc_numpy_support POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE:turbodbc_numpy_support> ${CMAKE_BINARY_DIR})

if (APPLE)
   set_target_properties(turbodbc_numpy_support PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()


install(
    TARGETS turbodbc_numpy_support
    DESTINATION lib
)

install(
    DIRECTORY turbodbc_numpy
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".git" EXCLUDE
)

install(
    DIRECTORY src/
    DESTINATION src/turbodbc_numpy
    FILES_MATCHING PATTERN "*.cpp"
    PATTERN ".git" EXCLUDE
)
