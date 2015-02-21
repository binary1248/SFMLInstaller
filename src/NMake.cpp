#include <NMake.hpp>
#include <System.hpp>
#include <cstdlib>
#include <iostream>

namespace nmake {

std::string get_vs_vars_path_from_env_var();

std::string get_vs_vars_path( bool probe ) {
	std::string vsvars_path = ( sys::try_execute( "vsvars32.bat" ) ? "vsvars32.bat" : "" );

	if( !vsvars_path.empty() ) {
		std::cout << "vsvars32.bat found through PATH.\n";
		std::cout << vsvars_path << "\n";
		return vsvars_path;
	}
	else {
		vsvars_path = get_vs_vars_path_from_env_var();
	}

	if( !vsvars_path.empty() ) {
		std::cout << "vsvars32.bat found through environment variable.\n";
		std::cout << vsvars_path << "\n";
		return vsvars_path;
	}
	else if( !probe ) {
		vsvars_path = sys::get_path_from_user( { "vsvars32.bat" } );
	}
	else {
		return "";
	}

	if( !vsvars_path.empty() ) {
		std::cout << "vsvars32.bat found through user.\n";
		std::cout << vsvars_path << "\n";
		return vsvars_path;
	}

	return "";
}

std::string get_nmake_path( bool probe ) {
	std::string nmake_path = ( sys::try_execute( "nmake" ) ? "nmake" : "" );

	if( !nmake_path.empty() ) {
		std::cout << "nmake found through PATH.\n";
		std::cout << nmake_path << "\n";
		return nmake_path;
	}
	else {
		nmake_path = get_vs_vars_path_from_env_var();
	}

	if( !nmake_path.empty() ) {
		nmake_path += " && nmake";

		std::cout << "nmake found through environment variable.\n";
		std::cout << nmake_path << "\n";
		return nmake_path;
	}
	else if( !probe ) {
		nmake_path = sys::get_path_from_user( { "vsvars32.bat" } );
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

std::string get_vs_vars_path_from_env_var() {
	std::string nmake_path;

	auto try_get_env = [&]( const std::string& name ) {
		auto value = std::getenv( name.c_str() );

		if( !value ) {
			std::cout << name << " not found.\n";
		}
		else {
			std::string string_value = value;
			string_value += "vsvars32.bat";

			if( sys::try_execute( string_value ) ) {
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

}
