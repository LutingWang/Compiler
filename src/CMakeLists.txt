cmake_minimum_required(VERSION 3.7.1)

project (compiler)

aux_source_directory (. SRCS)
add_executable (compiler ${SRCS})
