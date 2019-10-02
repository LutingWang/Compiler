cmake_minimum_required (VERSION 3.7.1)

project (compiler)

add_subdirectory ("grammar")
set (LIBS ${LIBS} grammar)

add_subdirectory ("symtable")
set (LIBS ${LIBS} symtable)

aux_source_directory (. SRCS)
add_executable (compiler ${SRCS})
target_link_libraries (compiler ${LIBS})
