
cmake_minimum_required(VERSION 2.8)

add_subdirectory(../source "${CMAKE_CURRENT_BINARY_DIR}/source")
include_directories(../source)

file(GLOB TESTS Test.*.c*)

add_executable(test-runner
	UnitTest.cpp ${TESTS}
)

file(COPY
	dump-backtrace
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

target_link_libraries(test-runner ${CORELIBS} TaggedFormat)
