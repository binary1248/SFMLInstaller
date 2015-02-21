#pragma once

#include <string>

namespace nmake {

std::string get_vs_vars_path( bool probe = false );
std::string get_nmake_path( bool probe = false );

}
