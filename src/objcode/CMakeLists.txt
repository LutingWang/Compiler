cmake_minimum_required (VERSION 3.7.1)

project (objcode)

add_subdirectory ("RegPool")
set (LIBS ${LIBS} regpool)

add_subdirectory ("StackFrame")
set (LIBS ${LIBS} stackframe)

aux_source_directory (. SRCS)
add_library (objcode ${SRCS})
target_link_libraries (objcode ${LIBS})
