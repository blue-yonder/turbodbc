find_package(Boost REQUIRED COMPONENTS system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(odbc REQUIRED)
include_directories(SYSTEM ${Odbc_INCLUDE_DIR})

include_directories(Library)
include_directories(../cpp_odbc/Library)
include_directories(../turbodbc/Library)
add_subdirectory(Library)
