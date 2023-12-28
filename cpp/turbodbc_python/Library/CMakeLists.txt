file(GLOB_RECURSE LIBRARY_FILES "src/*.cpp")

pybind11_add_module(turbodbc_python ${LIBRARY_FILES})

set_target_properties(turbodbc_python PROPERTIES 
    PREFIX ""
    OUTPUT_NAME turbodbc_intern
    INSTALL_RPATH "../../.."
)

add_dependencies(turbodbc_python
    refresh_cmake_configuration
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
