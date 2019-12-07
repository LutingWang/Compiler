cmake_minimum_required (VERSION 3.7.1)

project (compiler)

add_subdirectory ("datastream")
set (LIBS ${LIBS} datastream)

add_subdirectory ("frontend")
set (LIBS ${LIBS} frontend)

add_subdirectory ("objcode")
set (LIBS ${LIBS} objcode)

add_subdirectory ("optimizer")
set (LIBS ${LIBS} optimizer)

aux_source_directory (. SRCS)
add_executable (compiler ${SRCS})
target_link_libraries (compiler ${LIBS})
