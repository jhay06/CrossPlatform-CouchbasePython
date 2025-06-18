cmake_minimum_required(VERSION 3.16)
project(couchbase_lite_cffi CXX)
set(CMAKE_CXX_STANDARD 17)  
# Set your Couchbase Lite C SDK path
message(${CMAKE_CURRENT_SOURCE_DIR}) 
set(CBL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/mac/libcblite/include")
set(CBL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/mac/libcblite/lib")

# Shared library output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
file(GLOB_RECURSE CouchbaseSources RELATIVE  "${CMAKE_CURRENT_SOURCE_DIR}"
    "src/*.cpp"
    "src/*.h"
)
add_library(couchbase_lite_cffi SHARED
    ${CouchbaseSources}
)

target_include_directories(couchbase_lite_cffi PRIVATE
    ${CBL_INCLUDE_DIR}
)

target_link_directories(couchbase_lite_cffi PRIVATE
    ${CBL_LIB_DIR}
)

target_link_libraries(couchbase_lite_cffi
    cblite
)

# Optional: Rename output to .so for Python module use
set_target_properties(couchbase_lite_cffi PROPERTIES
    OUTPUT_NAME "couchbase_lite_cffi"
    PREFIX ""  # remove "lib" prefix for cffi
)
