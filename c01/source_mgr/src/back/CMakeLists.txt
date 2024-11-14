file(GLOB SRC "*.cpp")

set(trg ${CMAKE_PROJECT_NAME})
add_executable(${trg} ${SRC})

ChgExeOutputDir(${trg})

set(LLVM_CMAKE_PATH /home/lin17/Workspace/project/llvm_install)
find_package(LLVM REQUIRED HINTS "${LLVM_CMAKE_PATH}")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${trg}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${trg}>"
	${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(LLVM_LIBRARIES core support irreader)

message("LLVM LIB: ${LLVM_LIBRARIES}")
target_link_libraries(${trg}
	${LLVM_LIBRARIES}
)

#include(Doxygen)
#Doxygen("${CMAKE_CURRENT_SOURCE_DIR}/app" "${CMAKE_SOURCE_DIR}/docs")
#
#include(Install)

