#include <Make.hpp>
#include <System.hpp>
#include <Introspection.hpp>
#include <iostream>

std::string GetMakePath( bool probe ) {
	std::string make_path = ( TryExecute( "make" ) ? "make" : "" );

	if( !make_path.empty() ) {
		std::cout << "make found through PATH.\n";
		std::cout << make_path << "\n";
		return make_path;
	}
	else {
		make_path = ( TryExecute( "mingw32-make" ) ? "mingw32-make" : "" );
	}

	if( !make_path.empty() ) {
		std::cout << "make found through PATH.\n";
		std::cout << make_path << "\n";
		return make_path;
	}
	else {
		make_path = ( TryExecute( "mingw64-make" ) ? "mingw64-make" : "" );
	}

	if( !make_path.empty() ) {
		std::cout << "make found through PATH.\n";
		std::cout << make_path << "\n";
		return make_path;
	}
	else {
		make_path = GetMakePathFromIntrospection();
	}

	if( !make_path.empty() ) {
		std::cout << "make found through registry.\n";
		std::cout << make_path << "\n";
		return make_path;
	}
	else if( !probe ) {
		make_path = GetPathFromUser( { "make.exe", "mingw32-make.exe" } );
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
