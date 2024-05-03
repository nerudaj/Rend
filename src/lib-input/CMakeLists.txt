cmake_minimum_required ( VERSION 3.19 )

get_latest_clang_format()

project ( lib-input )

glob_sources_and_headers ( SRCS HDRS )

add_library (
	${PROJECT_NAME} STATIC
	${SRCS} ${HDRS}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${PROJECT_NAME} lib-options Dep_dgm Dep_json )

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )
