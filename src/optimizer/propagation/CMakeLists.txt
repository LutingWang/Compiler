cmake_minimum_required (VERSION 3.7.1)

project (propagation)

aux_source_directory (. SRCS)
add_library (propagation ${SRCS})
