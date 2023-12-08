cmake_minimum_required ( VERSION 3.26 )

get_latest_clang_format()

project ( tests )

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestHelpers/TestAssets.hpp.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestHelpers/TestAssets.hpp"
)

glob_sources_and_headers ( HEADERS SOURCES )

add_executable ( ${PROJECT_NAME}
	${SOURCES} ${HEADERS}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries( ${PROJECT_NAME}
	lib-app lib-game lib-editor Dep_catch Dep_fakeit
)

add_test (
	NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
