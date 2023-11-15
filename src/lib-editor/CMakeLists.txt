cmake_minimum_required ( VERSION 3.19 )

file (
	COPY "${CMAKE_BINARY_DIR}/.clang-format"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

project ( lib-editor )

glob_sources_and_headers ( HEADERS SOURCES )

add_library ( ${PROJECT_NAME} STATIC
	${SOURCES} ${HEADERS}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${PROJECT_NAME}
	lib-memory Dep_json Dep_dgm Dep_tgui
)

autoset_target_compile_options ( ${PROJECT_NAME} FALSE )

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
