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

# The language standard we use
add_definitions("-std=c++11")

# build shared instead of static libraries
set(BUILD_SHARED_LIBS TRUE)

# flags for all compilation modes
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# flags for Debug compilation mode
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -pedantic") # add pedantic here as it breaks Coco

# flags for Release compilation mode
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pedantic") # add pedantic here as it breaks Coco

# default setup for boost (find_packag(Boost) is still required)
# set(BOOST_LIBRARYDIR $ENV{BOOST_LIB_DIR})
# set(BOOST_INCLUDEDIR "${BOOST_LIBRARYDIR}/../include")
# set(Boost_NO_SYSTEM_PATHS ON)

# By default do not set RPATH in installed files. We copy them to multiple
# locations and they might later be packaged in a python wheel.
set(CMAKE_SKIP_INSTALL_RPATH ON)

# Always enable CTest (add the BUILD_TESTING variable)
include(CTest)

# This target allows to enforce CMake refresh for a given target that uses glob
# to determine its source files.
add_custom_target(refresh_cmake_configuration
	ALL # execute on default make
	touch ${CMAKE_PARENT_LIST_FILE} # make cmake detect configuration is changed on NEXT build
	COMMENT "Forcing refreshing of the CMake configuration. This allows to use globbing safely."
)

