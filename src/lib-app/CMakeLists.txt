cmake_minimum_required ( VERSION 3.19 )

project ( lib-app )

get_latest_clang_format()
glob_sources_and_headers( HEADERS SOURCES )

add_library ( ${PROJECT_NAME} STATIC
	${HEADERS} ${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${PROJECT_NAME}
	lib-game lib-editor lib-resources lib-network Dep_tgui lib-gui-builder Dep_base64
)

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )
