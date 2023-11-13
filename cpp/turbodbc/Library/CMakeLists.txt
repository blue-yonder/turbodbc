file(GLOB_RECURSE LIBRARY_FILES "src/*.cpp")

add_library(turbodbc ${LIBRARY_FILES})

set_target_properties(turbodbc PROPERTIES 
    INSTALL_RPATH "../../.."
)

add_dependencies(turbodbc
    refresh_cmake_configuration
)

target_link_libraries(turbodbc
	${Boost_LIBRARIES}
    ${Simdutf_LIBRARIES}
	${Odbc_LIBRARIES}
	cpp_odbc
)

add_custom_command(TARGET turbodbc POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE:turbodbc> ${CMAKE_BINARY_DIR})

install(
    TARGETS turbodbc
    DESTINATION lib
)

install(
	DIRECTORY turbodbc
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".git" EXCLUDE
)

install(
	DIRECTORY src/
	DESTINATION src/turbodbc
	FILES_MATCHING PATTERN "*.cpp"
	PATTERN ".git" EXCLUDE
)
