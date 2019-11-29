cmake_minimum_required (VERSION 3.7.1)

project (optimizer)

add_subdirectory ("propagation")
set (LIBS ${LIBS} propagation)

aux_source_directory (. SRCS)
add_library (optimizer ${SRCS})
target_link_libraries (optimizer ${LIBS})
