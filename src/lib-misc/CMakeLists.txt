cmake_minimum_required ( VERSION 3.26 )

get_latest_clang_format()

project ( lib-misc )

glob_sources_and_headers ( SRCS HDRS )

add_library (
	${PROJECT_NAME} INTERFACE
	${SRCS} ${HDRS}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
