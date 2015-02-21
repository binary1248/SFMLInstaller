#if !defined( _WIN32 )
#error This file should not be built on a non-Win32 system.
#endif

#include <Introspection.hpp>
#include <System.hpp>
#include <windows.h>
#include <iostream>

namespace introspection {

std::string get_cmake_path_from_introspection() {
	auto get_path = []( HKEY root ) -> std::string {
		HKEY kitware_key;

		auto result = RegOpenKeyEx( root, "SOFTWARE\\Kitware", 0, KEY_READ | KEY_WOW64_32KEY, &kitware_key );

		if( result != ERROR_SUCCESS ) {
			std::cout << "RegOpenKeyEx error: " << result << "\n";
			return "";
		}

		auto cmake_keyname = sys::create_zeroed<TCHAR, 256>();

		result = RegEnumKey( kitware_key, 0, cmake_keyname.data(), cmake_keyname.size() );

		if( result != ERROR_SUCCESS ) {
			std::cout << "RegEnumKey error: " << result << "\n";
			return "";
		}

		if( std::string( cmake_keyname.data() ).find( "CMake " ) == std::string::npos ) {
			return "";
		}

		auto cmake_path = sys::create_zeroed<TCHAR, 65536>();
		LONG cmake_path_size = cmake_path.size();

		result = RegQueryValue( kitware_key, cmake_keyname.data(), cmake_path.data(), &cmake_path_size );

		if( result != ERROR_SUCCESS ) {
			std::cout << "RegQueryValue error: " << result << "\n";
			return "";
		}

		result = RegCloseKey( kitware_key );

		if( result != ERROR_SUCCESS ) {
			std::cout << "RegCloseKey error: " << result << "\n";
		}

		return std::string( cmake_path.data() );
	};

	std::string result;

	result = get_path( HKEY_CURRENT_USER );

	if( result.empty() ) {
		result = get_path( HKEY_LOCAL_MACHINE );
	}

	if( !result.empty() ) {
		result += "/bin/cmake";
	}

	return result;
}

std::string get_make_path_from_introspection() {
	auto get_path = []( HKEY root ) -> std::string {
		HKEY codeblocks_key;

		auto result = RegOpenKeyEx( root, "SOFTWARE\\CodeBlocks", 0, KEY_READ | KEY_WOW64_32KEY, &codeblocks_key );

		if( result != ERROR_SUCCESS ) {
			std::cout << "RegOpenKeyEx error: " << result << "\n";
			return "";
		}

		auto make_path = sys::create_zeroed<TCHAR, 65536>();
		DWORD make_path_size = make_path.size();

		result = RegQueryValueEx( codeblocks_key, "Path", nullptr, nullptr, reinterpret_cast<LPBYTE>( make_path.data() ), &make_path_size );

		if( result != ERROR_SUCCESS ) {
			std::cout << "RegQueryValueEx error: " << result << "\n";
			return "";
		}

		result = RegCloseKey( codeblocks_key );

		if( result != ERROR_SUCCESS ) {
			std::cout << "RegCloseKey error: " << result << "\n";
		}

		return std::string( make_path.data() );
	};

	std::string result;

	result = get_path( HKEY_CURRENT_USER );

	if( result.empty() ) {
		result = get_path( HKEY_LOCAL_MACHINE );
	}

	if( !result.empty() ) {
		result += "/MinGW/bin/make";

		result = sys::try_execute( result );
	}

	return result;
}

std::string get_nmake_path_from_introspection() {
	return "";
}

}
