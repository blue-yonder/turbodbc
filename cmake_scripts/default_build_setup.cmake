# This script contains a few options which are common to all projects we build

# automatically select a proper build type
# By default set debug, but use build_config environment variable
set(BUILD_TYPE_HELP_MESSAGE "Choose the type of build, options are: Debug Release")

if(NOT CMAKE_BUILD_TYPE)
    if("$ENV{build_config}" STREQUAL "Hudson")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "${BUILD_TYPE_HELP_MESSAGE}" FORCE)
    else()
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "${BUILD_TYPE_HELP_MESSAGE}" FORCE)
    endif()
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(BUILD_SHARED_LIBS TRUE)

option(BUILD_COVERAGE
       "Determines whether C++ files will be built with support for coverage measurement"
       OFF)
option(DISABLE_CXX11_ABI
       "Determines whether the C++ compiler shall link to a C++11 conforming standard library, see https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html for details"
       OFF)

if (UNIX)
  # flags apply for both Linux and OSX!
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  if (DISABLE_CXX11_ABI)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -pedantic")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pedantic")

  if (BUILD_COVERAGE)
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
      set(CMAKE_EXE_LINKER_FLAGS_DEBUG "--coverage")
      set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "--coverage")
      set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "--coverage")
      set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "--coverage")
  endif()
  add_definitions("-std=c++17")
else()
  set(gtest_force_shared_crt ON)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "TRUE")
  add_definitions("/std:c++17")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc -DNOMINMAX")
endif()

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# By default do not set RPATH in installed files. We copy them to multiple
# locations and they might later be packaged in a python wheel.
set(CMAKE_SKIP_INSTALL_RPATH ON)

# Always enable CTest (add the BUILD_TESTING variable)
include(CTest)

# This target allows to enforce CMake refresh for a given target that uses glob
# to determine its source files.
add_custom_target(refresh_cmake_configuration
	ALL # execute on default make
	cmake -E touch ${CMAKE_PARENT_LIST_FILE} # make cmake detect configuration is changed on NEXT build
	COMMENT "Forcing refreshing of the CMake configuration. This allows to use globbing safely."
)
