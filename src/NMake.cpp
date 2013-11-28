#include <cstdlib>
#include <iostream>
#include <NMake.hpp>
#include <System.hpp>

std::string GetNMakePathFromEnvVar();

std::string GetNMakePath( bool probe ) {
	std::string nmake_path = ( TryExecute( "nmake" ) ? "nmake" : "" );

	if( !nmake_path.empty() ) {
		std::cout << "nmake found through PATH.\n";
		std::cout << nmake_path << "\n";
		return nmake_path;
	}
	else {
		nmake_path = GetNMakePathFromEnvVar();
	}

	if( !nmake_path.empty() ) {
		std::cout << "nmake found through environment variable.\n";
		std::cout << nmake_path << "\n";
		return nmake_path;
	}
	else if( !probe ) {
		nmake_path = GetPathFromUser( "vsvars32.bat" );
	}
	else {
		return "";
	}

	if( !nmake_path.empty() ) {
		nmake_path += " && nmake";

		std::cout << "nmake found through user.\n";
		std::cout << nmake_path << "\n";
		return nmake_path;
	}

	return "";
}

std::string GetNMakePathFromEnvVar() {
	std::string nmake_path;

	auto try_get_env = [&]( const std::string& name ) {
		auto value = std::getenv( name.c_str() );

		if( !value ) {
			std::cout << name << " not found.\n";
		}
		else {
			std::string string_value = value;
			string_value += "vsvars32.bat";

			if( TryExecute( string_value ) ) {
				nmake_path = string_value;
			}
		}
	};

	try_get_env( "VS120COMNTOOLS" );
	if( !nmake_path.empty() ) {
		return nmake_path;
	}

	try_get_env( "VS110COMNTOOLS" );
	if( !nmake_path.empty() ) {
		return nmake_path;
	}

	try_get_env( "VS100COMNTOOLS" );
	if( !nmake_path.empty() ) {
		return nmake_path;
	}

	try_get_env( "VS90COMNTOOLS" );
	if( !nmake_path.empty() ) {
		return nmake_path;
	}

	try_get_env( "VS80COMNTOOLS" );
	if( !nmake_path.empty() ) {
		return nmake_path;
	}

	return "";
}
