cmake_minimum_required(VERSION 3.1.0)

if(${WIN32})
	if(${VACATIONTIME_LTO})
		message("-- LTO enabled on all compilations")
		set(VACATIONTIME_LTO_FLAGS "/LTCG /GL")
    endif()
    set(VACATIONTIME_EXTRA_FLAGS "/Ox /Ot")
    add_compile_options(/DGTEST_LANG_CXX11)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    	add_compile_options(/MDd)
    else()
    	add_compile_options(/MD)
    endif()
else(${WIN32})
	if(${VACATIONTIME_LTO})
		message("-- LTO enabled on all compilations")
		set(VACATIONTIME_LTO_FLAGS "-flto")
	endif()
	set(VACATIONTIME_EXTRA_FLAGS "-O3")
endif(${WIN32})
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${VACATIONTIME_EXTRA_FLAGS} ${VACATIONTIME_LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${VACATIONTIME_EXTRA_FLAGS} ${VACATIONTIME_LTO_FLAGS}")
set(CMAKE_LINK_FLAGS_RELEASE "${CMAKE_LINK_FLAGS_RELEASE} ${VACATIONTIME_EXTRA_FLAGS} ${VACATIONTIME_LTO_FLAGS}")
set(CMAKE_LINK_FLAGS_RELWITHDEBINFO "${CMAKE_LINK_FLAGS_RELWITHDEBINFO} ${VACATIONTIME_EXTRA_FLAGS} ${VACATIONTIME_LTO_FLAGS}")

set(BUILD_GTEST ON CACHE BOOL "Build GTEST shared library")
set(BUILD_GMOCK OFF CACHE BOOL "Build GMOCK shared library")

add_subdirectory(gtest)

if(${WIN32})
    add_compile_options(/Zc:rvalueCast /std:c++latest /D_HAS_AUTO_PTR_ETC)
else(${WIN32})
	add_compile_options(-std=c++1z -Wall -Wextra -Wpedantic -Wno-unknown-warning-option -Wno-unknown-pragmas -Wstrict-aliasing=3 -Wdisabled-optimization -Wcast-align -Wcast-qual -Wformat=2 -Winit-self -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wconversion -Wno-sign-conversion -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default)
endif(${WIN32})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(libvacationdb)
add_subdirectory(vacationtime)

if(${WIN32})
	file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user" 
	    "<?xml version=\"1.0\" encoding=\"utf-8\"?>     \
	    <Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
	    <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">
	        <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
	        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	    </PropertyGroup>
	    <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|x64'\">
	        <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
	        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	    </PropertyGroup>
	    <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">
	        <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
	        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	    </PropertyGroup>
	    <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|x64'\">
	        <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
	        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	    </PropertyGroup>
	    </Project>")
endif(${WIN32})
