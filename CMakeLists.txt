cmake_minimum_required ( VERSION 3.26 )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Compiled" )

# Download some helper files
file (
	DOWNLOAD
		"https://raw.githubusercontent.com/nerudaj/clang-format/main/.clang-format"
		"${CMAKE_BINARY_DIR}/.clang-format"
)

include ( "${CMAKE_SOURCE_DIR}/cmake/cpp.cmake" )
include ( "${CMAKE_SOURCE_DIR}/cmake/settings.cmake" )
include ( "${CMAKE_SOURCE_DIR}/cmake/version.cmake" )
include ( "${CMAKE_SOURCE_DIR}/cmake/utils.cmake" )
include ( "${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake" )

project ( "${SETTINGS_PROJECT_NAME}" )

option ( DISABLE_TESTING    "Disable unit test projects" OFF )
option ( IS_NIGHTBUILD      "Use name nightbuild.zip for the output package" OFF )
option ( MAKE_INSTALLER "Use NSIS for packaging" OFF )

if ( NOT ${DISABLE_TESTING})
	enable_testing()
endif()

add_subdirectory ( "src/lib-memory" )
add_subdirectory ( "src/lib-defines" )
add_subdirectory ( "src/lib-resources" )
add_subdirectory ( "src/lib-options" )
add_subdirectory ( "src/lib-editor" )
add_subdirectory ( "src/lib-game" )
add_subdirectory ( "src/lib-app" )
add_subdirectory ( "src/bin" )
add_subdirectory ( "src/demo-ordered-dither" )

if ( NOT ${DISABLE_TESTING} )
	add_subdirectory ( "src/tests" )
endif()

# Packaging
install (
	DIRECTORY   "resources"
	DESTINATION "."
	# TODO: This doesn't work
	# FILES_MATCHING PATTERN "xcf" EXCLUDE
)

install (
	DIRECTORY   "devtools"
	DESTINATION "."
)

if ( ${IS_NIGHTBUILD} )
	set ( CPACK_GENERATOR "ZIP" )
	set ( CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}" )
	set ( CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}" )
	set ( CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}" )
	set ( CPACK_PACKAGE_FILE_NAME           "nightbuild" )
else()
	if ( ${MAKE_INSTALLER} )
		include(InstallRequiredSystemLibraries)
		set ( CPACK_GENERATOR "NSIS" )
		set ( CPACK_PACKAGE_NAME "${SETTINGS_PROJECT_NAME}" )
		set (
			CPACK_PACKAGE_VERSION
			"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
		)
		set ( CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}" )
		set ( CPACK_PACKAGE_VENDOR "Jakub Neruda")
		set ( CPACK_NSIS_MODIFY_PATH "OFF")
		set ( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON" )
		set (
			CMAKE_MODULE_PATH
			"${PROJECT_SOURCE_DIR}/nsis"
			${CMAKE_MODULE_PATH}
		)
		configure_file(
			${PROJECT_SOURCE_DIR}/nsis/NSIS.definitions.nsh.in
			${CMAKE_CURRENT_BINARY_DIR}/nsis/NSIS.definitions.nsh
		)
	else()
		set ( CPACK_GENERATOR "ZIP" )
		set ( CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}" )
		set ( CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}" )
		set ( CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}" )
		set ( CPACK_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${PROJECT_VERSION}-windows-vc16-${CMAKE_GENERATOR_PLATFORM}" )
	endif()
endif()

include ( CPack )
