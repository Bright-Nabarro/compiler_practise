cmake_minimum_required(VERSION 3.25)
project(parse_main LANGUAGES CXX VERSION 0.0.1)

option(DEBUG_MODE ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(DEBUG_MODE)
	set(CMAKE_BUILD_TYPE Debug)
else()
	set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
	add_compile_definitions(
		\W3
	)
else()
	add_compile_options(
		-Wall
		-Wextra
		-Wuninitialized
	)
endif()

enable_testing()
include(Utils)

add_subdirectory("src/back")

add_custom_target(cp_compile_commands ALL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_BINARY_DIR}/compile_commands.json
			${CMAKE_SOURCE_DIR}/compile_commands.json
	DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
)

