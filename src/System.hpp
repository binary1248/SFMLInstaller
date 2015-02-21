#pragma once

#include <atomic>
#include <string>
#include <vector>

namespace sys {

bool create_directory_if_required( const std::string& directory );

bool try_execute( const std::string& executable, bool show = false );

std::string get_path_from_user( const std::vector<std::string>& executables );

bool console_execute( const std::string& exec, const std::string& param, std::atomic_bool& completion_flag, std::atomic_bool& execution_flag );

std::string select_directory( const std::string& caption );

void remove_directory( const std::string& directory );

std::string get_working_directory();

void set_working_directory( const std::string& directory );

}

#include <System.inl>
