file(GLOB SRC "*.cpp")

set(trg ${CMAKE_PROJECT_NAME})
add_executable(${trg} ${SRC})

include(Utils)
ChgExeOutputDir(${trg})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${trg}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${trg}>"
)

separate_arguments(llvm_definitions NATIVE_COMMAND ${LLVM_DEFINITIONS})
message(STATUS "adding llvm definitions to exe: ${llvm_definitions}")

target_compile_definitions(${trg} PRIVATE
	${llvm_definitions}
)

llvm_map_components_to_libnames(llvm_libs
	support
	core
	irreader
)

target_link_libraries(${trg}
	front
)

#include(Doxygen)
#Doxygen("${CMAKE_CURRENT_SOURCE_DIR}/app" "${CMAKE_SOURCE_DIR}/docs")
#
#include(Install)

