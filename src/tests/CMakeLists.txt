cmake_minimum_required ( VERSION 3.26 )

get_latest_clang_format()

project ( tests )

glob_sources ( SOURCES )

add_executable ( ${PROJECT_NAME}
	${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
) 

target_include_directories ( ${PROJECT_NAME}
	PRIVATE "${CATCH_FOLDER}"
)

target_link_libraries( ${PROJECT_NAME}
	lib-game
)

add_test (
	NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
