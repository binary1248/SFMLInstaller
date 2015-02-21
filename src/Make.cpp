#include <Make.hpp>
#include <System.hpp>
#include <Introspection.hpp>
#include <iostream>

namespace make {

std::string get_make_path( bool probe ) {
	std::string make_path = ( sys::try_execute( "make" ) ? "make" : "" );

	if( !make_path.empty() ) {
		std::cout << "make found through PATH.\n";
		std::cout << make_path << "\n";
		return make_path;
	}
	else {
		make_path = ( sys::try_execute( "mingw32-make" ) ? "mingw32-make" : "" );
	}

	if( !make_path.empty() ) {
		std::cout << "make found through PATH.\n";
		std::cout << make_path << "\n";
		return make_path;
	}
	else {
		make_path = ( sys::try_execute( "mingw64-make" ) ? "mingw64-make" : "" );
	}

	if( !make_path.empty() ) {
		std::cout << "make found through PATH.\n";
		std::cout << make_path << "\n";
		return make_path;
	}
	else {
		make_path = introspection::get_make_path_from_introspection();
	}

	if( !make_path.empty() ) {
		std::cout << "make found through registry.\n";
		std::cout << make_path << "\n";
		return make_path;
	}
	else if( !probe ) {
		make_path = sys::get_path_from_user( { "make.exe", "mingw32-make.exe" } );
	}
	else {
		return "";
	}

	if( !make_path.empty() ) {
		std::cout << "make found through user.\n";
		std::cout << make_path << "\n";
		return make_path;
	}

	return "";
}

}
