#pragma once

#include <string>

namespace decompress {

bool decompress_archive( const char* data, std::size_t size, const std::string& directory );

}
