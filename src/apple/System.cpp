#if !defined( __APPLE__ ) || !defined( __MACH__ )
#error This file should not be built on a non-apple system.
#endif

#include "System.hpp"

namespace sys {

bool create_directory_if_required( const std::string& directory ) {
	return false;
}

bool try_execute( const std::string& executable, bool show ) {
	return false;
}

std::string get_path_from_user( const std::string& executable ) {
	return "";
}

bool console_execute( const std::string& exec, const std::string& param, std::atomic_bool& completion_flag, std::atomic_bool& execution_flag ) {
	return false;
}

std::string select_directory( const std::string& caption ) {
	return "";
}

void remove_directory( const std::string& directory ) {
	return;
}

std::string get_working_directory() {
	return "";
}

void set_working_directory( const std::string& directory ) {
	return;
}

}

#error Unimplemented
