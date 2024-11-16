file(GLOB src "*.cpp")

set(trg ${CMAKE_PROJECT_NAME})

AddLLVMTrgExe(${trg} ${src})
ChgExeOutputDir(${trg})

#include(Doxygen)
#Doxygen("${CMAKE_CURRENT_SOURCE_DIR}/app" "${CMAKE_SOURCE_DIR}/docs")
#
#include(Install)

