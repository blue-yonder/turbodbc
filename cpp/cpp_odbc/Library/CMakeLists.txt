if (WIN32)
    include (GenerateExportHeader)
endif()

file(GLOB_RECURSE LIBRARY_FILES "src/*.cpp")

add_library(cpp_odbc ${LIBRARY_FILES})

add_dependencies(cpp_odbc
    refresh_cmake_configuration
)

target_link_libraries(cpp_odbc
    ${Boost_LIBRARIES}
    ${Simdutf_LIBRARIES}
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
