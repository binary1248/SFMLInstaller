#pragma once

#include <atomic>
#include <string>
#include <vector>

bool CreateDirectoryIfRequired( const std::string& directory );

bool TryExecute( const std::string& executable, bool show = false );

std::string GetPathFromUser( const std::vector<std::string>& executables );

bool ConsoleExecute( const std::string& exec, const std::string& param, std::atomic_bool& completion_flag, std::atomic_bool& execution_flag );

std::string SelectDirectory( const std::string& caption );

void RemoveDirectory( const std::string& directory );

std::string GetWorkingDirectory();

void SetWorkingDirectory( const std::string& directory );

#include <System.inl>
