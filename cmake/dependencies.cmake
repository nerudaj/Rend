set ( DGM_LIB_VERSION     "2.3.1" )
set ( DGM_FSM_LIB_VERSION "1.3.0" )
set ( DSH_VERSION         "1.7.2" )
set ( SFML_VERSION        "2.6.1" )
set ( TGUI_VERSION        "1.1.0" )
set ( CATCH_VERSION       "2.10.2" )
set ( JSON_VERSION        "3.11.2" )
set ( CXXOPTS_VERSION     "3.1.1" )
set ( ENTT_VERSION        "3.12.2" )
set ( FAKEIT_VERSION      "2.3.2" )

set ( DGM_LIB_URL "https://github.com/nerudaj/dgm-lib/releases/download/v${DGM_LIB_VERSION}/dgm-lib-${DGM_LIB_VERSION}-winx64-vc17-x64-for-SFML-${SFML_VERSION}.zip" )
set ( DGM_FSM_LIB_URL "https://github.com/nerudaj/dgm-fsm-lib/releases/download/v2.0.0-rc3/dgm-fsm-lib-v2.0.0-rc3-Windows-MSVC-x64.zip" )
set ( DSH_URL   "https://github.com/nerudaj/dsh/releases/download/v${DSH_VERSION}/dsh-${DSH_VERSION}-vc16-64-bit.zip" )
set ( SFML_URL    "https://github.com/SFML/SFML/releases/download/${SFML_VERSION}/SFML-${SFML_VERSION}-windows-vc15-64-bit.zip" )
set ( TGUI_URL    "https://github.com/texus/TGUI/releases/download/v${TGUI_VERSION}/TGUI-${TGUI_VERSION}-vc17-64bit-for-SFML-${SFML_VERSION}.zip" )
set ( CATCH_URL "https://github.com/catchorg/Catch2/releases/download/v${CATCH_VERSION}/catch.hpp" )
set ( JSON_URL  "https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/include.zip" )
set ( CXXOPTS_URL "https://github.com/jarro2783/cxxopts/archive/refs/tags/v${CXXOPTS_VERSION}.zip" )
set ( ENTT_URL "https://github.com/skypjack/entt/archive/refs/tags/v${ENTT_VERSION}.zip" )
set ( FAKEIT_URL "https://github.com/eranpeer/FakeIt/archive/refs/tags/${FAKEIT_VERSION}.zip" )
set ( BASE64_URL "https://raw.githubusercontent.com/tobiaslocker/base64/master/include/base64.hpp" )

include ( FetchContent )

function ( fetch_dependency name url headeronly )
	string ( TOLOWER ${name} lname )

	if ( ${headeronly} )	
		FetchContent_Declare ( ${name}
			URL                  ${url}
			DOWNLOAD_NO_PROGRESS TRUE
			DOWNLOAD_NO_EXTRACT  TRUE
		)
	else ()
		FetchContent_Declare ( ${name}
			URL ${url}
			DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		)
	endif ()
	
	FetchContent_GetProperties ( ${name} )
	if ( NOT "${${name}_POPULATED}" )
		FetchContent_Populate ( ${name} )
		set ( "${name}_POPULATED" TRUE PARENT_SCOPE )
	endif ()
	
	set ( "${name}_FOLDER" "${${lname}_SOURCE_DIR}" PARENT_SCOPE )
endfunction ()

# Download dependencies
fetch_dependency ( SFML ${SFML_URL}    FALSE )
fetch_dependency ( DSH  ${DSH_URL}     FALSE )
fetch_dependency ( DGM  ${DGM_LIB_URL} FALSE )
fetch_dependency ( DGM_FSM  ${DGM_FSM_LIB_URL} FALSE )
fetch_dependency ( TGUI ${TGUI_URL}    FALSE )
fetch_dependency ( CATCH ${CATCH_URL} TRUE )
fetch_dependency ( JSON  ${JSON_URL}  FALSE )
fetch_dependency ( CXXOPTS ${CXXOPTS_URL} FALSE )
fetch_dependency ( ENTT ${ENTT_URL} FALSE )
fetch_dependency ( FAKEIT ${FAKEIT_URL} FALSE )
fetch_dependency ( BASE64 ${BASE64_URL} TRUE )

# Verify folder paths
message ( "Dependencies downloaded to: " )
message ( "  DGM:   ${DGM_FOLDER}" )
message ( "  FSM:   ${DGM_FSM_FOLDER}" )
message ( "  DSH:   ${DSH_FOLDER}" )
message ( "  SFML:  ${SFML_FOLDER}" )
message ( "  TGUI:  ${TGUI_FOLDER}" )
message ( "  CATCH: ${CATCH_FOLDER}" )
message ( "  JSON:  ${JSON_FOLDER}" )
message ( "  OPTS:  ${CXXOPTS_FOLDER}" )
message ( "  ENTT:  ${ENTT_FOLDER}" )
message ( "  FAKEIT: ${FAKEIT_FOLDER}" )
message ( "  BASE64: ${BASE64_FOLDER}" )

# Make libraries visible to cmake linker
link_directories("${DSH_FOLDER}/lib")
link_directories("${DGM_FOLDER}/lib")
link_directories("${DGM_FSM_FOLDER}/lib")
link_directories("${SFML_FOLDER}/lib")
link_directories("${TGUI_FOLDER}/lib")

# Create symbols for linking libcfg, libstrings, libleveld and
message ( "Looking for dsh libs" )
find_library(LIB_CFG_D config-d  NAMES config-d.lib  HINTS "${DSH_FOLDER}/lib")
find_library(LIB_STR_D strings-d NAMES strings-d.lib HINTS "${DSH_FOLDER}/lib")
find_library(LIB_LVLD_D leveld-d NAMES leveld-d.lib HINTS "${DSH_FOLDER}/lib")

find_library(LIB_CFG_R config  NAMES config.lib  HINTS "${DSH_FOLDER}/lib")
find_library(LIB_STR_R strings NAMES strings.lib HINTS "${DSH_FOLDER}/lib")
find_library(LIB_LVLD_R leveld NAMES leveld.lib HINTS "${DSH_FOLDER}/lib")

set(LIB_CFG optimized ${LIB_CFG_R} debug ${LIB_CFG_D})
set(LIB_STR optimized ${LIB_STR_R} debug ${LIB_STR_D})
set(LIB_LVLD optimized ${LIB_LVLD_R} debug ${LIB_LVLD_D})
message ( "OK" )

message ( "Looking for libdgm" )
find_library(LIB_DGM_D libdgm-d  NAMES libdgm-d.lib  HINTS "${DGM_FOLDER}/lib")
if ( "${LIB_DGM_D}" EQUAL "LIB_DGM_D-NOTFOUND" )
	message ( FATAL_ERROR "Cannot find libdgm-d.lib" )
endif()

find_library(LIB_DGM_R libdgm  NAMES libdgm.lib  HINTS "${DGM_FOLDER}/lib")
if ( "${LIB_DGM_R}" EQUAL "LIB_DGM_R-NOTFOUND" )
	message ( FATAL_ERROR "Cannot find libdgm.lib" )
endif()

set(LIB_DGM optimized ${LIB_DGM_R} debug ${LIB_DGM_D})
message ( "OK" )

message ( "Looking for libfsm" )
find_library(LIB_DGM_FSM_D libfsm-d  NAMES libfsm-d.lib  HINTS "${DGM_FSM_FOLDER}/lib")
if ( "${LIB_DGM_FSM_D}" EQUAL "LIB_DGM_FSM_D-NOTFOUND" )
	message ( FATAL_ERROR "Cannot find libfsm-d.lib" )
endif()

find_library(LIB_DGM_FSM_R libfsm  NAMES libfsm.lib  HINTS "${DGM_FSM_FOLDER}/lib")
if ( "${LIB_DGM_FSM_R}" EQUAL "LIB_DGM_FSM_R-NOTFOUND" )
	message ( FATAL_ERROR "Cannot find libfsm.lib" )
endif()

set(LIB_DGM_FSM optimized ${LIB_DGM_FSM_R} debug ${LIB_DGM_FSM_D})
message ( "OK" )

message ( "Looking for SFML" )
find_library ( LIB_SFML_MAIN_D "sfml-main-d" NAMES "sfml-main-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_SYSTEM_D "sfml-system-d" NAMES "sfml-system-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_GRAPHICS_D "sfml-graphics-d" NAMES "sfml-graphics-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_WINDOW_D "sfml-window-d" NAMES "sfml-window-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_AUDIO_D "sfml-audio-d" NAMES "sfml-audio-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_NETWORK_D "sfml-network-d" NAMES "sfml-network-d.lib" HINTS "${SFML_FOLDER}/lib" )

find_library ( LIB_SFML_MAIN_R "sfml-main" NAMES "sfml-main.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_SYSTEM_R "sfml-system" NAMES "sfml-system.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_GRAPHICS_R "sfml-graphics" NAMES "sfml-graphics.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_WINDOW_R "sfml-window" NAMES "sfml-window.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_AUDIO_R "sfml-audio" NAMES "sfml-audio.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_NETWORK_R "sfml-network" NAMES "sfml-network.lib" HINTS "${SFML_FOLDER}/lib" )

set ( LIB_SFML_MAIN optimized ${LIB_SFML_MAIN_R} debug ${LIB_SFML_MAIN_D} )
set ( LIB_SFML_SYSTEM optimized ${LIB_SFML_SYSTEM_R} debug ${LIB_SFML_SYSTEM_D} )
set ( LIB_SFML_GRAPHICS optimized ${LIB_SFML_GRAPHICS_R} debug ${LIB_SFML_GRAPHICS_D} )
set ( LIB_SFML_WINDOW optimized ${LIB_SFML_WINDOW_R} debug ${LIB_SFML_WINDOW_D} )
set ( LIB_SFML_AUDIO optimized ${LIB_SFML_AUDIO_R} debug ${LIB_SFML_AUDIO_D} )
set ( LIB_SFML_NETWORK optimized ${LIB_SFML_NETWORK_R} debug ${LIB_SFML_NETWORK_D} )
set ( LIB_SFML ${LIB_SFML_MAIN} ${LIB_SFML_SYSTEM} ${LIB_SFML_GRAPHICS} ${LIB_SFML_WINDOW} ${LIB_SFML_AUDIO} ${LIB_SFML_NETWORK} )
message ( "OK" )

message ( "Looking for TGUI" )
find_library ( LIB_TGUI_D "tgui-d" NAMES "tgui-d.lib" HINTS "${TGUI_FOLDER}/lib" )
find_library ( LIB_TGUI_R "tgui" NAMES "tgui.lib" HINTS "${TGUI_FOLDER}/lib" )

set ( LIB_TGUI optimized ${LIB_TGUI_R} debug ${LIB_TGUI_D} )
message ( "OK" )

add_library ( Dep_sfml INTERFACE )
target_include_directories ( Dep_sfml INTERFACE "${SFML_FOLDER}/include" )
target_link_libraries ( Dep_sfml INTERFACE ${LIB_SFML} )

add_library ( Dep_lvd INTERFACE )
target_include_directories ( Dep_lvd INTERFACE "${DSH_FOLDER}/include" )
target_link_libraries ( Dep_lvd INTERFACE ${LIB_LVLD} )

add_library ( Dep_dgm INTERFACE )
target_include_directories ( Dep_dgm INTERFACE "${DGM_FOLDER}/include" )
target_link_libraries ( Dep_dgm INTERFACE ${LIB_DGM} Dep_sfml Dep_lvd xinput.lib )

add_library ( Dep_dgm_fsm INTERFACE )
target_include_directories ( Dep_dgm_fsm INTERFACE "${DGM_FSM_FOLDER}/include" )
target_link_libraries ( Dep_dgm_fsm INTERFACE ${LIB_DGM_FSM} )

add_library ( Dep_tgui INTERFACE )
target_include_directories ( Dep_tgui INTERFACE "${TGUI_FOLDER}/include" )
target_link_libraries ( Dep_tgui INTERFACE ${LIB_TGUI} )

add_library ( Dep_json INTERFACE )
target_include_directories ( Dep_json INTERFACE "${JSON_FOLDER}/include" )

add_library ( Dep_opts INTERFACE )
target_include_directories ( Dep_opts INTERFACE "${CXXOPTS_FOLDER}/include" )

add_library ( Dep_catch INTERFACE )
target_include_directories ( Dep_catch INTERFACE "${CATCH_FOLDER}" )

add_library ( Dep_fakeit INTERFACE )
target_include_directories ( Dep_fakeit INTERFACE "${FAKEIT_FOLDER}/single_header/standalone" )

add_library ( Dep_base64 INTERFACE )
target_include_directories ( Dep_base64 INTERFACE "${BASE64_FOLDER}" )