find_path(Simdutf_INCLUDE_DIRS simdutf.h $ENV{CONDA_PREFIX}/Library/include)
find_library(Simdutf_LIBRARIES simdutf  $ENV{CONDA_PREFIX}/Library/lib)
include_directories(SYSTEM ${Simdutf_INCLUDE_DIRS})

find_package(odbc REQUIRED)
include_directories(SYSTEM ${Odbc_INCLUDE_DIR})

include_directories(Library)
add_subdirectory(Library)

if(BUILD_TESTING)
    add_subdirectory(Test)
endif()
