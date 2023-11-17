set ( CMAKE_GENERATOR_PLATFORM     x64)
set ( CMAKE_CXX_STANDARD		   23 )
set ( CMAKE_CXX_STANDARD_REQUIRED  ON )
set ( CMAKE_CXX_EXTENSIONS         OFF )
set ( MY_CXX_COMPILE_FEATURES      cxx_generic_lambdas cxx_range_for cxx_strong_enums )
set ( CXX_COMPILE_OPTIONS          /W4 /MP /we4265 /we4834 /we4456 /wd4251 /we5205 )
# C4265 - Missing virtual dtor
# C4834 - Discarding result of [[nodiscard]] function
# C4456 - Name shadowing
# C5205 - Dtor on iface is not virtual

if (NOT CMAKE_DEBUG_POSTFIX)
	set(CMAKE_DEBUG_POSTFIX "-d")
endif()