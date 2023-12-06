cmake_minimum_required ( VERSION 3.26 )

get_latest_clang_format()

project ( tests )

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestHelpers/TestAssets.hpp.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestHelpers/TestAssets.hpp"
)

file (
	GLOB HEADERS
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.hpp"
)

glob_sources ( SOURCES )

add_executable ( ${PROJECT_NAME}
	${SOURCES} ${HEADERS}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
) 

target_include_directories ( ${PROJECT_NAME}
	PRIVATE "${CATCH_FOLDER}"
)

target_link_libraries( ${PROJECT_NAME}
	lib-app lib-game lib-editor
)

target_precompile_headers ( ${PROJECT_NAME}
	PRIVATE
		<vector>
		<format>
		<map>
		<unordered_map>
		<array>
		<optional>
		<expected>
		<fstream>
		<filesystem>
		<functional>
		<concepts>
		<memory>
		<string>
		<cstdint>
		<DGM/dgm.hpp>
		<TGUI/tgui.hpp>
		<nlohmann/json.hpp>
)

add_test (
	NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
