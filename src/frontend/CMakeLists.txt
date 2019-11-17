cmake_minimum_required (VERSION 3.7.1)

project (frontend)

add_subdirectory ("grammar")
set (LIBS ${LIBS} grammar)

aux_source_directory (. SRCS)
add_library (frontend ${SRCS})
target_link_libraries (frontend ${LIBS})
