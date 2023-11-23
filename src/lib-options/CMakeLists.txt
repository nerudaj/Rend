cmake_minimum_required ( VERSION 3.26 )

project ( lib-options )

glob_modules ( SRCS )

add_library ( ${PROJECT_NAME} ${SRCS} )

target_link_libraries ( ${PROJECT_NAME} PUBLIC Dep_json Dep_sfml )

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )
