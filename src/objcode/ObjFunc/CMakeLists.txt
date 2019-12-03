cmake_minimum_required (VERSION 3.7.1)

project (objfunc)

aux_source_directory (. SRCS)
add_library (objfunc ${SRCS})
target_link_libraries (objfunc ${LIBS})
