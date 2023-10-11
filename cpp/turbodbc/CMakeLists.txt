find_package(Boost REQUIRED COMPONENTS system date_time locale)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(odbc REQUIRED)
include_directories(SYSTEM ${Odbc_INCLUDE_DIR})

include_directories(Library)
include_directories(../cpp_odbc/Library)
add_subdirectory(Library)

if(BUILD_TESTING)
    add_subdirectory(Test)
endif()
