cmake_minimum_required( VERSION 2.8 )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE )
endif()

project( SFMLInstaller )

set( SFML_STATIC_LIBRARIES TRUE CACHE BOOL "Choose the link type for SFML" FORCE )
set( SFGUI_STATIC_LIBRARIES TRUE CACHE BOOL "Choose the link type for SFGUI" FORCE )
set( SFNUL_STATIC_LIBRARIES TRUE CACHE BOOL "Choose the link type for SFNUL" FORCE )

if( MSVC OR CMAKE_COMPILER_IS_GNUCXX )
	set( SFMLInstaller_USE_STATIC_STD_LIBS TRUE CACHE BOOL "True to link the runtime library statically, false to link them dynamically." )
endif()

set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} )

find_package( SFML 2 REQUIRED COMPONENTS graphics window system )
find_package( SFGUI REQUIRED )
find_package( SFNUL REQUIRED )

if( APPLE )
	find_library( COREFOUNDATION_LIBRARY CoreFoundation )
	mark_as_advanced( COREFOUNDATION_LIBRARY )
endif()

set( SRC_DIR "${PROJECT_SOURCE_DIR}/src/" )

set( SYSTEM_DIR "" )

if( WIN32 )
	set( SYSTEM_DIR "win32" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
	set( SYSTEM_DIR "apple" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
	set( SYSTEM_DIR "linux" )
else()
	message( FATAL "Your system is currently unsupported." )
endif()

set(
	SOURCES
	"${SRC_DIR}/CMake.cpp"
	"${SRC_DIR}/CMake.hpp"
	"${SRC_DIR}/Decompress.cpp"
	"${SRC_DIR}/Decompress.hpp"
	"${SRC_DIR}/Installer.cpp"
	"${SRC_DIR}/Installer.hpp"
	"${SRC_DIR}/Introspection.hpp"
	"${SRC_DIR}/main.cpp"
	"${SRC_DIR}/Make.cpp"
	"${SRC_DIR}/Make.hpp"
	"${SRC_DIR}/NMake.cpp"
	"${SRC_DIR}/NMake.hpp"
	"${SRC_DIR}/SFMLInstaller.cpp"
	"${SRC_DIR}/SFMLInstaller.hpp"
	"${SRC_DIR}/System.hpp"
	"${SRC_DIR}/System.inl"
	"${SRC_DIR}/${SYSTEM_DIR}/CMakeVersion.cpp"
	"${SRC_DIR}/${SYSTEM_DIR}/Introspection.cpp"
	"${SRC_DIR}/${SYSTEM_DIR}/System.cpp"
)

include_directories( ${SRC_DIR} )
include_directories( "${SRC_DIR}/${SYSTEM_DIR}" )
include_directories( SYSTEM ${SFML_INCLUDE_DIR} )
include_directories( SYSTEM ${SFGUI_INCLUDE_DIR} )
include_directories( SYSTEM ${SFNUL_INCLUDE_DIR} )

if( APPLE )
	include_directories( /System/Library/Frameworks/CoreFoundation.framework/Headers )
endif()

set( GNUCXX_FLAGS "-Wall -Wextra -Wshadow -Wconversion -Wunused-parameter -Wno-long-long -Wunreachable-code -Wredundant-decls -Wcast-align -Wfloat-equal -Wformat=2 -Wmissing-declarations -Woverlength-strings -pedantic" )
set( GNULD_FLAGS "" )

if( SFMLInstaller_USE_STATIC_STD_LIBS )
	if( MSVC )
		foreach( flag CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO )
			if( ${flag} MATCHES "/MD" )
				string( REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}" )
			endif()
		endforeach()
	elseif( CMAKE_COMPILER_IS_GNUCXX )
		set( GNULD_FLAGS "-static" )
	endif()
endif()

add_executable( SFMLInstaller ${SOURCES} )

if( CMAKE_COMPILER_IS_GNUCXX )
	if( CMAKE_CXX_COMPILER MATCHES ".*clang[+][+]" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
		set( GNUCXX_FLAGS "-stdlib=libc++ ${GNUCXX_FLAGS}" )
	else()
		set( GNUCXX_FLAGS "-std=c++11 ${GNUCXX_FLAGS}" )
	endif()
	
	set_source_files_properties( ${SOURCES} PROPERTIES COMPILE_FLAGS ${GNUCXX_FLAGS} )
	set_target_properties( SFMLInstaller PROPERTIES LINK_FLAGS ${GNULD_FLAGS} )
endif()

target_link_libraries( SFMLInstaller ${SFNUL_LIBRARY} ${SFGUI_LIBRARY} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} )

if( WIN32 )
	target_link_libraries( SFMLInstaller ws2_32 comdlg32 ole32 )
elseif( APPLE )
	target_link_libraries( SFMLInstaller ${COREFOUNDATION_LIBRARY} )
endif()
