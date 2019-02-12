cmake_minimum_required(VERSION 3.10)
project(psst-math LANGUAGES CXX)

add_library(psst-math INTERFACE)

target_include_directories(psst-math
	INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

#target_compile_definitions(psst-math 
#	INTERFACE BOOST_ASIO_HEADER_ONLY=1)

#target_link_libraries(psst-math 
#	INTERFACE )
