#include <CMake.hpp>
#include <System.hpp>
#include <Introspection.hpp>
#include <iostream>

namespace cmake {

std::string get_cmake_path( bool probe ) {
	std::string cmake_path = ( sys::try_execute( "cmake" ) ? "cmake" : "" );

	if( !cmake_path.empty() ) {
		std::cout << "CMake found through PATH.\n";
		std::cout << cmake_path << "\n";
		return cmake_path;
	}
	else {
		cmake_path = introspection::get_cmake_path_from_introspection();
	}

	if( !cmake_path.empty() ) {
		std::cout << "CMake found through registry.\n";
		std::cout << cmake_path << "\n";
		return cmake_path;
	}
	else if( !probe ) {
		cmake_path = sys::get_path_from_user( { "cmake.exe" } );
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

}
