cmake_minimum_required ( VERSION 3.26 )

project ( lib-defines LANGUAGES CXX )

get_latest_clang_format()

add_library ( ${PROJECT_NAME} INTERFACE )

target_include_directories ( ${PROJECT_NAME}
	INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
