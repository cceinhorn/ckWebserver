# Collect all source files
aux_source_directory(. SRC_LIST)

# Generate shared library libmytestserver.so
# add_library(memorypool_lib SHARED ${SRC_LIST})
add_library(memorypool ${SRC_LIST})
