function(ChgExeOutputDir target)
	set_target_properties(${target} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
	)
endfunction()

macro(AddTrgLibrary name)
	if (TARGET ${name})
		message(WARNING "Target ${name} already exists. Skipping AddTrgLibrary")
		return()
	endif()

	set (LIBTYPE STATIC)
	if (BUILD_SHARED_LIBS)
		set(LIBTYPE SHARED)
	endif()

	llvm_add_library(${name} ${LIBTYPE} ${ARGN})
	target_link_libraries(${name} PUBLIC ${LLVM_COMMON_LIBS})
endmacro()

macro(AddTrgExe name)
	add_llvm_executable(${name} ${ARGN})
	#target_include_directories(${name} PUBLIC
	#	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${trg}>"
	#	"$<BUILD_INTERFACE:${LLVM_INCLUDE_DIRS}>"
	#    "$<INSTALL_INTERFACE:include>"
	#)
endmacro()


