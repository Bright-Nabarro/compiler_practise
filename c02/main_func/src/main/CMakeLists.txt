file(GLOB SRC "*.cpp")

set(trg ${CMAKE_PROJECT_NAME})

include(Utils)
AddTrgExe(${trg} ${SRC})

ChgExeOutputDir(${trg})

#target_link_libraries(


#include(Doxygen)
#Doxygen("${CMAKE_CURRENT_SOURCE_DIR}/app" "${CMAKE_SOURCE_DIR}/docs")
#
#include(Install)

