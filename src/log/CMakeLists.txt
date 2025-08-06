# Collect all source files
aux_source_directory(. SRC_LIST)

# Generate shared library libmytestserver.so
add_library(logger ${SRC_LIST})

