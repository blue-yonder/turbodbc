cmake_minimum_required(VERSION 3.10)
project(turbodbc_intern CXX)

# Take precedence over system modules
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake_scripts)

include(default_build_setup)

# Boost 1.70 comes with its own CMake config which is incompatible with
# using FindBoost.cmake from CMake. While we still support older Boost
# versions, rely on the CMake built-in.
set(Boost_NO_BOOST_CMAKE ON)

find_package(pybind11 CONFIG)
find_package(GTest)
if (GTest_FOUND OR GTEST_FOUND)
  include_directories(${GTEST_INCLUDE_DIR})
else()
  add_subdirectory(google_test)
  set(GTEST_LIBRARY gtest)
  set(GMOCK_LIBRARY gmock)
  set(GMOCK_MAIN_LIBRARY gmock_main)
endif()
add_subdirectory(cpp)
add_subdirectory(python)
add_subdirectory(docs)

if(BUILD_TESTING)
    enable_testing()
endif()

install(
	FILES setup.py setup.cfg MANIFEST.in Makefile LICENSE README.md CHANGELOG.rst pyproject.toml
	DESTINATION .
)
