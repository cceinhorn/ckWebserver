cmake_minimum_required(VERSION 3.10)
project(ckserver)

# Set compilation options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread")

# CMAKE_ARCHIVE_OUTPUT_DIRECTORY    .a/.lib
# CMAKE_LIBRARY_OUTPUT_DIRECTORY    .so/.dll
# CMAKE_RUNTIME_OUTPUT_DIRECTORY    .exe
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Add header file search paths
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(src/http)
add_subdirectory(src/log)
add_subdirectory(src/memorypool)
add_subdirectory(test)

