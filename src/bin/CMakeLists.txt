cmake_minimum_required ( VERSION 3.19 )

project ( "${SETTINGS_BINARY_NAME}" )

get_latest_clang_format()
copy_dlls_to_build_folders()
glob_sources_and_headers ( SOURCES HEADERS )

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/resources.rc.in"
	"${CMAKE_CURRENT_BINARY_DIR}/resources.rc"
)

# Create final executable
add_executable ( ${PROJECT_NAME} WIN32
	${SOURCES} ${HEADERS}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
	"${CMAKE_CURRENT_BINARY_DIR}/resources.rc"
)

target_include_directories ( ${PROJECT_NAME}
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${PROJECT_NAME}
	lib-app Dep_opts
)

set_target_properties ( ${PROJECT_NAME} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_dependencies ( ${PROJECT_NAME}
	lib-app lib-audio lib-resources lib-game lib-memory lib-options
)

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )

# Packaging
install (
	FILES       "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/${SETTINGS_BINARY_NAME}.exe"
	DESTINATION "bin"
)

file ( GLOB DLLS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/*.dll" )

install (
	FILES       ${DLLS}
	DESTINATION "bin"
)
