cmake_minimum_required (VERSION 3.7.1)

project (datastream)

aux_source_directory (. SRCS)
add_library (datastream ${SRCS})
