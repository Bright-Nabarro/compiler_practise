file(GLOB SRC "*.cpp")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(flex_file parser.l)
set(bison_file parser.y)

string(REPLACE ".l" ".flex.cpp" flex_output ${flex_file})
string(REPLACE ".y" ".bison.cpp" bison_output ${bison_file})

flex_target(lexer ${flex_file} ${flex_output})
bison_target(parser ${bison_file} ${bison_output}
	DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.bison.hpp)

add_flex_bison_dependency(lexer parser)

include(Utils)
AddTrgLibrary(front ${SRC} ${flex_output} ${bison_output})

target_include_directories(front PUBLIC
	"include"
	${CMAKE_CURRENT_BINARY_DIR}
)


