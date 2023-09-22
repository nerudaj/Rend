cmake_minimum_required ( VERSION 3.26 )

project ( lib-memory )

glob_modules ( SRCS )

add_library ( ${PROJECT_NAME} ${SRCS} )

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )
