file(GLOB SRC "*.cpp")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(flex_file lexer.ll)
set(bison_file parser.yy)

string(REPLACE ".ll" ".flex.cpp" flex_output ${flex_file})
string(REPLACE ".yy" ".bison.cpp" bison_output ${bison_file})

set(output_header ${CMAKE_CURRENT_BINARY_DIR}/bison_parser.hpp)
flex_target(lexer ${flex_file} ${flex_output})
bison_target(parser ${bison_file} ${bison_output}
	DEFINES_FILE ${output_header})

add_flex_bison_dependency(lexer parser)

include(Utils)
#add_library(front OBJECT ${SRC} ${flex_output} ${bison_output})
message("flex generate file:  ${flex_output}")
message("bison generate file:  ${bison_output}")
AddLLVMTrgLibrary(front OBJECT ${SRC} ${flex_output} ${bison_output})

target_include_directories(front PUBLIC
	"include"
	${CMAKE_CURRENT_BINARY_DIR}
)


include(Doxygen)
Doxygen("${CMAKE_SOURCE_DIR}/docs"
	"." "include" ${flex_output} ${bison_output} ${output_header}
	"${CMAKE_CURRENT_BINARY_DIR}/location.hh"
)

