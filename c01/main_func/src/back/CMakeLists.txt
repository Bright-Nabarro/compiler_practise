file(GLOB SRC "*.cpp")

set(trg ${CMAKE_PROJECT_NAME})
add_executable(${trg} ${SRC})

ChgExeOutputDir(${trg})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${trg}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${trg}>"
)

target_link_libraries(${trg}
	front
)

#include(Doxygen)
#Doxygen("${CMAKE_CURRENT_SOURCE_DIR}/app" "${CMAKE_SOURCE_DIR}/docs")
#
#include(Install)

