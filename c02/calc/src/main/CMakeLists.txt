file(GLOB SRC "*.cpp")

set(trg ${CMAKE_PROJECT_NAME})

include(Utils)
add_executable(${trg} ${SRC})

ChgExeOutputDir(${trg})
target_include_directories(${trg} PRIVATE
	"${CMAKE_SOURCE_DIR}/src/front/include"
)
target_link_libraries(${trg} PRIVATE
	front
)

#target_link_libraries(

#include(Doxygen)
#Doxygen("${CMAKE_CURRENT_SOURCE_DIR}/app" "${CMAKE_SOURCE_DIR}/docs")
#
#include(Install)

