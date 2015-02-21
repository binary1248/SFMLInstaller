#if !defined( __APPLE__ ) || !defined( __MACH__ )
#error This file should not be built on a non-apple system.
#endif

#include "Introspection.hpp"
#include "System.hpp"

namespace introspection {

std::string get_cmake_path_from_introspection() {
	return "";
}

std::string get_make_path_from_introspection() {
	return "";
}

std::string get_nmake_path_from_introspection() {
	return "";
}

}

#error Unimplemented
