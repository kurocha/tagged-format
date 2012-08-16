
set(CORELIBS stdc++)

add_library(TaggedModelFormat STATIC
	Reader.cpp
	Writer.cpp
	Block.cpp
	Parser.cpp
	MemoryBuffer.cpp
)

target_link_libraries(TaggedModelFormat ${CORELIBS})

install(
	TARGETS
		TaggedModelFormat
	ARCHIVE DESTINATION
		lib
)

add_executable(tmf-tool
	tmf-tool.cpp
)

target_link_libraries(tmf-tool ${CORELIBS} TaggedModelFormat)

install(
	TARGETS
		tmf-tool
	RUNTIME DESTINATION
		bin
)

install(
	FILES
		Block.h
		Buffer.h
		MemoryBuffer.h
		Parser.h
		Reader.h
		Writer.h
	DESTINATION "include/TaggedModelFormat/"
)

# Generate documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)
