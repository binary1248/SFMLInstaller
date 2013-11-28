#if !defined( __APPLE__ ) || !defined( __MACH__ )
#error This file should not be built on a non-apple system.
#endif

#include "Introspection.hpp"
#include "System.hpp"

std::string GetCMakePathFromIntrospection() {
	return "";
}

std::string GetMakePathFromIntrospection() {
	return "";
}

std::string GetNMakePathFromIntrospection() {
	return "";
}

#error Unimplemented
