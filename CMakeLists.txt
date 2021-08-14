# EDGE CMake Script
cmake_minimum_required(VERSION 3.2.2)

#EDGE IN THE DARK
project(EITD)

if( COMMAND cmake_policy )
	if( POLICY CMP0011 )
		cmake_policy( SET CMP0011 NEW )
	endif()
	if( POLICY CMP0054 )
		cmake_policy( SET CMP0054 NEW )
	endif()
endif()

IF (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
        MESSAGE( FATAL_ERROR "In-tree Builds are NOT supported.")
ENDIF (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

list( APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )
include( FindPackageHandleStandardArgs )

macro( make_release_only )
	set( CMAKE_C_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_RELEASE} )
	set( CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELEASE} )
	string( REPLACE "/MT " "/MTd " CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_RELEASE} )
	set( CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_RELEASE} )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELEASE} )
	string( REPLACE "/MT " "/MTd " CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_RELEASE} )
endmacro()

set( LZMA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/lzma/C" )
option(FORCE_STATIC_MSVC_LIBS "Use pre-build via ./libs" ON)
option(FORCE_INTERNAL_ZLIB "Use internal zlib")
option(FORCE_INTERNAL_SDL "Use internal SDL")
option(FORCE_INTERNAL_SDL_MIXER "Use internal SDL_MIXER")
if(MINGW)
	option(MINGW_LINK_STATIC "Link statically on MinGW" ON)
endif()

if(MINGW AND MINGW_LINK_STATIC)
	SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a)
endif()

find_package( ZLIB )
find_package(SDL2)
find_package(SDL_MIXER2)


if(APPLE)
find_library(COCOA_LIBRARY Cocoa)
FIND_PATH(COCOA_INCLUDE_DIR Cocoa/Cocoa.h)
endif()


if(NOT ZLIB_FOUND OR INTERNAL_ZLIB)
	message(STATUS "Using internal zlib")
	set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib)
	add_subdirectory(deps/zlib)
	set(ZLIB_LIBRARIES ZLIB::ZLIB)
	set(ZLIB_LIBRARY ZLIB::ZLIB)
else()
	message(STATUS "Using system zlib, includes found at ${ZLIB_INCLUDE_DIR}")
endif()


if(NOT SDL_MIXER_FOUND OR INTERNAL_SDL_MIXER)
	message(STATUS "Using internal SDL_MIXER")
	set(SDL_MIXER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL_mixer-1.2.12/include)
	add_subdirectory(deps/SDL_mixer-1.2.12)
	set(SDL_MIXER_LIBRARIES SDL_MIXER)
else()
	message(STATUS "Using system SDL_MIXER, includes found at ${SDL_MIXER_INCLUDE_DIRS}")
endif()

if(NOT SDL_FOUND OR INTERNAL_SDL)
	message(STATUS "Using internal SDL1")
	set(SDL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL-1.2.15/include)
	#add_subdirectory(deps/SDL-1.2.15)
	set(SDL_LIBRARIES SDL)
else()
	message(STATUS "Using system SDL, includes found at ${SDL_INCLUDE_DIRS}")
endif()



if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		set ( CMAKE_CXX_FLAGS "-mfpu=neon-vfpv4 -mtune=cortex-a7 -mhard-float ${CMAKE_CXX_FLAGS}" )
	elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
		set( CMAKE_CXX_FLAGS "-mcpu=cortex-a72 -mtune=cortex-a72 -march=armv8-a+fp+simd+crc ${CMAKE_CXX_FLAGS}" )
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4 AND NOT MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
	endif()
if(NOT MSVC)
	if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
		set ( CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}" )
	else ()
		set ( CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}" )
endif()
endif(NOT MSVC)

if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
    add_definitions(/W1)
endif()

include_directories(
        deps/miniz/
)
add_compile_definitions(EITD PRIVATE
        "MINIZ_NO_ARCHIVE_WRITING_APIS"
        "MINIZ_NO_TIME"
        )


add_definitions(
	-DINTERNAL_DEBUGGER
	#-DINIT_EPI
	#-DHAVE_PHYSFS
	#-DUSE_FFMPEG
)

# Removed HAVE_STDINT_H on Visual Studio/Win32 machines (Caused problems, so I Shoved it into UNIX define)
if(APPLE)
	add_definitions(
		-DMACOSX
		-DHAVE_STDINT_H
	)
elseif(UNIX)
	if(${CMAKE_SYSTEM_NAME} MATCHES "(Free|Open)BSD")
		add_definitions(
			-DBSD
			-DHAVE_STDINT_H
		)
		else()
			add_definitions(
			-DLINUX
			-DHAVE_STDINT_H
		)
	endif()
endif()

# Module Paths
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/modules)


#######
######################### Find Needed Libs #####################################

# See /VSLibs/lib_versions.txt [libs have been compiled with MSVC 2017!]
# CA: 11/30/18 - For MSVC, libs are no longer required to be downloaded seperately, they now exist in /libs/msvc/
if (FORCE_STATIC_MSVC_LIBS)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set( SDL_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL-1.2.15/lib/x64/SDL.lib" )
		set( SDLMAIN_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL-1.2.15/lib/x64/SDLmain.lib" )
		set( SDL_MIXER_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL_mixer-1.2.12/lib/x64/SDL_mixer.lib" )
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set( SDL_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL-1.2.15/lib/x86/SDL.lib" )
		set( SDLMAIN_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL-1.2.15/lib/x86/SDLmain.lib" )
		set( SDL_MIXER_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL_mixer-1.2.12/lib/x86/SDL_mixer.lib" )
	endif()
endif(FORCE_STATIC_MSVC_LIBS)
#message(${CMAKE_MODULE_PATH})

# OpenGL
if (NOT VITA)
    if(POLICY CMP0072)
        cmake_policy(SET CMP0072 NEW)
    endif()
    FIND_PACKAGE (OpenGL)
elseif(VITA)
    FIND_PACKAGE (VitaGL)
endif()

# libvorbis/libogg

# zlib
#find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})

include_directories(${SDL_INCLUDE_DIRS})

include_directories(${SDL_MIXER_INCLUDE_DIR})

# dumb
#include_directories(${LIBDUMB_INCLUDE_DIR})

# EDGE
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/sse2neon)
set(EITD_SOURCES
src/actor.cc
src/actor_list.cc
src/aitd_box.cc
src/anim.cc
src/anim_action.cc
src/cos_table.cc
src/debugger.cc
src/debug_font.cc
src/eval_var.cc
src/filestream.cc
src/file_access.cc
src/fitd.cc
src/floor.cc
src/fmopl.cc
src/font.cc
src/gfx.cc
src/hqr.cc
src/input.cc
src/inventory.cc
src/life.cc
src/life_2.cc
src/life_macro_table.cc
src/lines.cc
src/main.cc
src/main_loop.cc
src/music.cc
src/object.cc
src/osystemSDL.cc
src/osystemSDL_GL.cc
src/pak.cc
src/palette.cc
src/polys.cc
src/renderer.cc
src/resource.cc
src/room.cc
src/save.cc
src/screen.cc
src/sequence.cc
src/startup_menu.cc
src/system_menu.cc
src/tatou.cc
src/textes.cc
src/thread_code.cc
src/timer.cc
src/track.cc
src/unpack.cc
src/vars.cc
src/version.cc
src/video_mode.cc
src/zv.cc
#src/common/textconsole.cc, subbed instead for w32_system.cc
)

set(EITD_WIN32_SOURCES
	#src/system/win32/w32_music.cc
	#src/system/win32/w32_mus.cc
	#src/system/win32/w32_net.cc
	src/system/win32/w32_system.cc
	#src/system/win32/i_exception.cc
	#epi/epi_win32.cc
	#epi/filesystem_win32.cc
)


set(EITD_UNIX_SOURCES
    #	src/system/unx/unx_main.cc
	#src/system/unx/unx_music.cc
	#src/system/unx/unx_net.cc
	#src/system/unx/unx_system.cc
	#epi/filesystem_linux.cc
	#epi/epi_linux.cc
)

set(EITD_DREAMCAST_SOURCES

)

#if (APPLE)
#	set(EITD_UNIX_SOURCES
#		${EITD_UNIX_SOURCES}
#		src/system/unx/mac/mac_ui.mm
#	}
#endif()

if(WIN32)
	set(EITD_SOURCES ${EITD_SOURCES}
		${EITD_WIN32_SOURCES}
	)
	if(MSVC)
		set(EITD_SOURCES ${EITD_SOURCES}
			src/system/win32/w32_manifest.manifest
		)
	else()
		set(EITD_SOURCES ${EITD_SOURCES}
			src/system/win32/w32_manifest_mgw.rc
		)	
	endif()
elseif(UNIX)
	set(EITD_SOURCES ${EITD_SOURCES}
		${EITD_UNIX_SOURCES}
	)
elseif(DREAMCAST)
	set(EITD_SOURCES ${EITD_SOURCES}
		${EITD_DREAMCAST_SOURCES}
	)
elseif(VITA)
	set(EITD_SOURCES ${EITD_SOURCES}
		${EITD_VITA_SOURCES}
	)
endif()

add_executable ( EITD ${EITD_SOURCES} )

target_link_libraries(EITD
	${SDL_LIBRARIES}
	${SDL_MIXER_LIBRARIES}
	#Ogg::ogg
	#Vorbis::vorbis Vorbis::vorbisenc Vorbis::vorbisfile
#	${VORBIS_LIBRARIES} ${VORBISFILE_LIBRARIES}
	ZLIB::ZLIB
	#${PHYSFS_LIBRARY}
	#	${LIBDUMB_LIBRARY}
	#	${SDLKitchensink_STATIC_LIBRARY}
)

if(MINGW)
	set(MINGW_BUILD_EITD_TARGET_PROPERTIES "-Wl,-subsystem,windows")
	if(MINGW_LINK_STATIC)
		set(MINGW_BUILD_EITD_TARGET_PROPERTIES "${MINGW_BUILD_EDGE_TARGET_PROPERTIES} -static-libgcc -static-libstdc++ -static")
	endif()
	set_target_properties(EITD PROPERTIES LINK_FLAGS "${MINGW_BUILD_EDGE_TARGET_PROPERTIES}")
endif()

if (WIN32)
	target_link_libraries(EITD
		wsock32
		winmm
		gdi32
		opengl32
		glu32
		Imm32
		Setupapi
		Version
	)
elseif(APPLE)
	set_target_properties(EITD PROPERTIES LINK_FLAGS "-framework OpenGL -framework Cocoa -framework IOKit")
	target_link_libraries(EITD SDL2 COCOA_LIBRARY stdc++ "-framework Foundation" "-framework Cocoa" "-framework IOKIT" objc)
elseif(VITA)
	target_link_libraries(EITD SDL2 vitaGL SceDisplay_stub SceCtrl_stub SceAudio_stub SceSysmodule_stub SceGxm_stub SceCommonDialog_stub SceTouch_stub SceHid_stub m)
elseif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "(Free|Open)BSD")
	target_link_libraries(EITD SDL2 OpenGL::GL dl)
endif()

#if(MSVC)
#   target_link_libraries(EDGE rpcrt4)
#endif()

INSTALL (TARGETS EITD
        RUNTIME DESTINATION ${BIN_DIR}
        LIBRARY DESTINATION ${LIB_DIR}
        ARCHIVE DESTINATION ${LIB_DIR})

## EOF
#target_link_libraries( ajbsp )