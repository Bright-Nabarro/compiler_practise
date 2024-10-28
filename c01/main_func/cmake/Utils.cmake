function(ChgExeOutputDir target)
	set_target_properties(${target} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
	)
endfunction()

