
set(CORELIBS stdc++)

add_library(TaggedFormat STATIC
	Reader.cpp
	Writer.cpp
	Block.cpp
	External.cpp
	Table.cpp
	Axes.cpp
	Mesh.cpp
	Parser.cpp
	MemoryBuffer.cpp
)

target_link_libraries(TaggedFormat ${CORELIBS})

install(
	TARGETS
		TaggedFormat
	ARCHIVE DESTINATION
		lib
)

add_executable(tf-convert
	tf-convert.cpp
)

target_link_libraries(tf-convert ${CORELIBS} TaggedFormat)

install(
	TARGETS
		tf-convert
	RUNTIME DESTINATION
		bin
)

install(
	FILES
		Block.h
		External.h
		Table.h
		Axes.h
		Mesh.h
		Buffer.h
		MemoryBuffer.h
		Parser.h
		Reader.h
		Writer.h
		Alignment.h
	DESTINATION "include/TaggedFormat/"
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
