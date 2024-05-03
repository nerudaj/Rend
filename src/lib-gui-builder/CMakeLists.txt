cmake_minimum_required ( VERSION 3.19 )

file (
	COPY "${CMAKE_BINARY_DIR}/.clang-format"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

project ( lib-gui-builder )

glob_sources_and_headers ( SRCS HDRS )

add_library (
	${PROJECT_NAME} STATIC
	${SRCS} ${HDRS}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${PROJECT_NAME} lib-memory Dep_tgui Dep_dgm Dep_sfml lib-defines )

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )
