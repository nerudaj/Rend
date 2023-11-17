cmake_minimum_required ( VERSION 3.19 )

file (
	COPY "${CMAKE_BINARY_DIR}/.clang-format"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

project ( lib-filesystem )

glob_sources_and_headers ( HEADERS SOURCES )

add_library ( ${PROJECT_NAME} STATIC
	${SOURCES} ${HEADERS}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )
