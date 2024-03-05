cmake_minimum_required ( VERSION 3.26 )

project ( benchmark )

get_latest_clang_format()

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/include/RootDir.hpp.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/RootDir.hpp"
)

add_executable ( ${PROJECT_NAME} 
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/RootDir.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${PROJECT_NAME} lib-game )

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )