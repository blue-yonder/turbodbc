file(GLOB_RECURSE LIBRARY_FILES "src/*.cpp")

# We need to disable LTO on Arrow builds
pybind11_add_module(turbodbc_arrow_support SHARED ${LIBRARY_FILES} NO_EXTRAS)

set_target_properties(turbodbc_arrow_support PROPERTIES
    PREFIX ""
    INSTALL_RPATH "../../.."
)

add_dependencies(turbodbc_arrow_support
    refresh_cmake_configuration
)

target_link_libraries(turbodbc_arrow_support
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC ${Odbc_LIBRARIES}
    PUBLIC cpp_odbc
    PUBLIC turbodbc
)

if (MSVC)
    target_link_libraries(turbodbc_arrow_support
        PUBLIC ${ARROW_SHARED_IMP_LIB}
        PUBLIC ${ARROW_PYTHON_SHARED_IMP_LIB}
    )
else()
    target_link_libraries(turbodbc_arrow_support
        PUBLIC ${ARROW_SHARED_LIB}
        PUBLIC ${ARROW_PYTHON_SHARED_LIB}
    )
endif()

add_custom_command(TARGET turbodbc_arrow_support POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE:turbodbc_arrow_support> ${CMAKE_BINARY_DIR})

if (APPLE)
   set_target_properties(turbodbc_arrow_support PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

install(
    TARGETS turbodbc_arrow_support
    DESTINATION lib
)

install(
	DIRECTORY turbodbc_arrow
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".git" EXCLUDE
)

install(
	DIRECTORY src/
	DESTINATION src/turbodbc_arrow
	FILES_MATCHING PATTERN "*.cpp"
	PATTERN ".git" EXCLUDE
)

