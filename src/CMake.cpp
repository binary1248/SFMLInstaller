#include <CMake.hpp>
#include <System.hpp>
#include <Introspection.hpp>
#include <iostream>

std::string GetCMakePath( bool probe ) {
	std::string cmake_path = ( TryExecute( "cmake" ) ? "cmake" : "" );

	if( !cmake_path.empty() ) {
		std::cout << "CMake found through PATH.\n";
		std::cout << cmake_path << "\n";
		return cmake_path;
	}
	else {
		cmake_path = GetCMakePathFromIntrospection();
	}

	if( !cmake_path.empty() ) {
		std::cout << "CMake found through registry.\n";
		std::cout << cmake_path << "\n";
		return cmake_path;
	}
	else if( !probe ) {
		cmake_path = GetPathFromUser( { "cmake.exe" } );
	}
	else {
		return "";
	}

	if( !cmake_path.empty() ) {
		std::cout << "CMake found through user.\n";
		std::cout << cmake_path << "\n";
		return cmake_path;
	}

	return "";
}
