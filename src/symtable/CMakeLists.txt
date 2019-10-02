cmake_minimum_required (VERSION 3.7.1)

project (symtable)

aux_source_directory (. SRCS)
add_library (symtable ${SRCS})
