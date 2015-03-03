project(GLEW)
cmake_minimum_required(VERSION 2.8)

option(GLEW_INSTALL "Generate installation target" ON)

add_definitions(-DGLEW_BUILD -DGLEW_NO_GLU)
include_directories(include)
add_library(glew STATIC src/glew.c)

if( GLEW_INSTALL )
	install(TARGETS glew ARCHIVE DESTINATION lib)
	install(DIRECTORY include/GL DESTINATION include)
endif()