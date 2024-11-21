set(LLVM_LINK_COMPONENTS
	${LLVM_TARGETS_TO_BUILD}
	Core
	Support
	Irreader
)

#add_subdirectory(front)
add_subdirectory(main)

