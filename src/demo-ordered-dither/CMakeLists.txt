cmake_minimum_required ( VERSION 3.26 )

project ( demo-ordered-dither )

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/RootDir.hpp.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/RootDir.hpp"
)

add_executable ( ${PROJECT_NAME} Main.cpp )

target_link_libraries ( ${PROJECT_NAME} Dep_sfml )
