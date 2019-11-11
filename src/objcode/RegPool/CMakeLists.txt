cmake_minimum_required (VERSION 3.7.1)

project (regpool)

aux_source_directory (. SRCS)
add_library (regpool ${SRCS})
