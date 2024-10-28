find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(FLEX_INPUT )
set(BISION_INPUT ) 

string(REPLACE ".l" ".cpp" FLEX_OUTPUT ${FLEX_INPUT})
string(REPLACE ".l" ".cpp" BISON_OUTPUT ${BISON_INPUT})

message(STATUS "flex output source file: ${FLEX_OUTPUT}")
message(STATUS "bison output source file: ${BISON_OUTPUT}")

flex_target(lexer ${FLEX_INPUT} ${FLEX_OUTPUT})
binson_target(parser ${BISON_INPUT} ${BISON_OUTPUT})

add_flex_bison_dependency(lexer parser)

add_library(front OBJECT ${BISON_OUTPUT} ${FLEX_OUTPUT})

target_link_libraries(front ${FLEX_LIBRARIES})

# 导入bison生成的.hpp文件
target_include_directories(front ${CMAKE_CURRENT_BINARY_DIR})

