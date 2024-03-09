cmake_minimum_required ( VERSION 3.26 )

get_latest_clang_format()

project ( client-test )

add_executable ( ${PROJECT_NAME} Main.cpp )

target_link_libraries ( ${PROJECT_NAME} lib-network lib-misc )

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )