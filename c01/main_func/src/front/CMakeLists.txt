find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(FLEX_INPUT parser.l)
set(BISON_INPUT parser.y) 

string(REPLACE ".l" ".flex.cpp" FLEX_OUTPUT ${FLEX_INPUT})
string(REPLACE ".y" ".bison.cpp" BISON_OUTPUT ${BISON_INPUT})

message(STATUS "flex output source file: ${FLEX_OUTPUT}")
message(STATUS "bison output source file: ${BISON_OUTPUT}")

flex_target(lexer ${FLEX_INPUT} ${FLEX_OUTPUT})
bison_target(parser ${BISON_INPUT} ${BISON_OUTPUT}
			 DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.bison.hpp)

add_flex_bison_dependency(lexer parser)

add_library(front OBJECT ${BISON_OUTPUT} ${FLEX_OUTPUT})

target_link_libraries(front ${FLEX_LIBRARIES})

# 导入bison生成的.hpp文件
target_include_directories(front PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	${CMAKE_CURRENT_BINARY_DIR}
)

