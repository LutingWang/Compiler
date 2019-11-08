cmake_minimum_required (VERSION 3.7.1)

project (optimizer)

aux_source_directory (. SRCS)
add_library (optimizer ${SRCS})
