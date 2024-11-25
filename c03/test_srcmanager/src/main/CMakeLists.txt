file(GLOB src "*.cpp")

set(trg ${CMAKE_PROJECT_NAME})

include(AddLLVM)
include(Utils)

set(LLVM_LINK_COMPONENTS
	${LLVM_TARGETS_TO_BUILD}
	Core
	Support
	Irreader
)

AddLLVMTrgExe(${trg} ${src})

ChgExeOutputDir(${trg})

#include(Doxygen)
#Doxygen("${CMAKE_CURRENT_SOURCE_DIR}/app" "${CMAKE_SOURCE_DIR}/docs")
#
#include(Install)

