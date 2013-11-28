#if !defined( __linux__ )
#error This file should not be built on a non-linux system.
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
