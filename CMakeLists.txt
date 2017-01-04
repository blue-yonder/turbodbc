cmake_minimum_required(VERSION 2.8.7)
project(turbodbc_intern CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_scripts)

include(default_build_setup)

add_subdirectory(google_test)
add_subdirectory(cpp)
add_subdirectory(python)
add_subdirectory(docs)

if(BUILD_TESTING)
    enable_testing()
endif()

install(
	FILES setup.py setup.cfg MANIFEST.in Makefile
	DESTINATION .
)
