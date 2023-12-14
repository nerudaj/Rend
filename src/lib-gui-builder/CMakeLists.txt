cmake_minimum_required ( VERSION 3.19 )

file (
	COPY "${CMAKE_BINARY_DIR}/.clang-format"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

project ( lib-gui-builder )

glob_modules ( SRCS )

add_library ( ${PROJECT_NAME} ${SRCS} )

target_link_libraries ( ${PROJECT_NAME} lib-memory Dep_tgui Dep_dgm Dep_sfml lib-defines )

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )
