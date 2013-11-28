#if !defined( __APPLE__ ) || !defined( __MACH__ )
#error This file should not be built on a non-apple system.
#endif

#include "System.hpp"

bool CreateDirectoryIfRequired( const std::string& directory ) {
	return false;
}

bool TryExecute( const std::string& executable, bool show ) {
	return false;
}

std::string GetPathFromUser( const std::string& executable ) {
	return "";
}

bool ConsoleExecute( const std::string& exec, const std::string& param, std::atomic_bool& completion_flag, std::atomic_bool& execution_flag ) {
	return false;
}

std::string SelectDirectory( const std::string& caption ) {
	return "";
}

void RemoveDirectory( const std::string& directory ) {
	return;
}

std::string GetWorkingDirectory() {
	return "";
}

void SetWorkingDirectory( const std::string& directory ) {
	return;
}

#error Unimplemented
