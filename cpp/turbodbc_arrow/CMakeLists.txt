find_package(Boost REQUIRED COMPONENTS system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_path(Simdutf_INCLUDE_DIRS simdutf.h $ENV{CONDA_PREFIX}/Library/include)
find_library(Simdutf_LIBRARIES simdutf  $ENV{CONDA_PREFIX}/Library/lib)
include_directories(SYSTEM ${Simdutf_INCLUDE_DIRS})

include_directories(SYSTEM ${PYTHON_INCLUDE_DIRS})

find_package(odbc REQUIRED)
include_directories(SYSTEM ${Odbc_INCLUDE_DIR})

find_package(numpy REQUIRED)
include_directories(SYSTEM ${Numpy_INCLUDE_DIR})

find_package(Arrow REQUIRED)
include_directories(SYSTEM ${ARROW_INCLUDE_DIR} ${PYARROW_INCLUDE_DIR})
add_library(arrow SHARED IMPORTED)
set_target_properties(arrow PROPERTIES IMPORTED_LOCATION ${ARROW_SHARED_LIB})
add_library(arrow_python SHARED IMPORTED)
set_target_properties(arrow_python PROPERTIES IMPORTED_LOCATION ${ARROW_PYTHON_SHARED_LIB})

include_directories(Library)
include_directories(../cpp_odbc/Library)
include_directories(../turbodbc/Library)
add_subdirectory(Library)

if(BUILD_TESTING)
    add_subdirectory(Test)
endif()
