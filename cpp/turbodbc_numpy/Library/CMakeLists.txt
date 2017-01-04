file(GLOB_RECURSE LIBRARY_FILES "src/*.cpp")

add_library(turbodbc_numpy_support ${LIBRARY_FILES})

set_target_properties(turbodbc_numpy_support PROPERTIES 
    PREFIX ""
    INSTALL_RPATH "../../.."
)

add_dependencies(turbodbc_numpy_support
    refresh_cmake_configuration
)

target_link_libraries(turbodbc_numpy_support
	${Boost_LIBRARIES}
	${Odbc_LIBRARIES}
	cpp_odbc
	turbodbc
)

add_custom_command(TARGET turbodbc_numpy_support POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_BINARY_DIR}/turbodbc_numpy_support${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_BINARY_DIR})

if (APPLE)
   set_target_properties(turbodbc_numpy_support PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

install(
    TARGETS turbodbc_numpy_support
    LIBRARY DESTINATION lib
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
