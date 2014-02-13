#if !defined( _WIN32 )
#error This file should not be built on a non-Win32 system.
#endif

#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <System.hpp>

bool CreateDirectoryIfRequired( const std::string& directory ) {
	if( directory.length() > 247 ) {
		std::cout << "CreateDirectoryIfRequired error: Path too long.\n";
	}

	auto pos = directory.find( '/' );

	while( pos != std::string::npos ) {
		std::string current_segment = directory.substr( 0, pos );

		if( !CreateDirectory( current_segment.c_str(), nullptr ) ) {
			auto error = GetLastError();

			if( error != ERROR_ALREADY_EXISTS ) {
				std::cout << "CreateDirectoryIfRequired couldn't create directory (" << current_segment << "): " << error << "\n";
				return false;
			}
		}

		pos = directory.find( '/', pos + 1 );
	}

	return true;
}

bool TryExecute( const std::string& executable, bool show ) {
	auto result = reinterpret_cast<int>( ShellExecute( nullptr, "open", executable.c_str(), "", nullptr, show ? SW_SHOW : SW_HIDE ) );

	std::cout << "Trying to execute " << executable << ": " << result << "\n";

	if( result > 32 ) {
		return true;
	}

	return false;
}

std::string GetPathFromUser( const std::vector<std::string>& executables ) {
	auto strTitle = std::string{ "Where is " };

	if( executables.empty() ) {
		std::cout << "No filter specified.\n";
		return "";
	}
	else if( executables.size() == 1 ) {
		strTitle += executables.front();
	}
	else {
		auto end = std::end( executables );
		for( auto iter = std::begin( executables ); ; ) {
			strTitle += *iter;

			++iter;

			if( iter != end ) {
				strTitle += " or ";
			}
			else {
				break;
			}
		}
	}

	strTitle += "?";

	auto strFilter = std::string{ "" };

	for( const auto& e : executables ) {
		strFilter += e + '\0' + e + '\0';
	}

	auto filename = CreateZeroed<TCHAR, 65536>();
	auto open_filename = CreateZeroed<OPENFILENAME>();

	open_filename.lStructSize = sizeof( OPENFILENAME );
	open_filename.lpstrFilter = strFilter.c_str();
	open_filename.lpstrFile = filename.data();
	open_filename.nMaxFile = filename.size();
	open_filename.lpstrTitle = strTitle.c_str();
	open_filename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	auto working_directory = GetWorkingDirectory();

	auto result = GetOpenFileName( &open_filename );

	SetWorkingDirectory( working_directory );

	if( !result ) {
		return "";
	}

	return std::string( filename.data() );
}

bool ConsoleExecute( const std::string& exec, const std::string& param, std::atomic_bool& completion_flag, std::atomic_bool& execution_flag ) {
	auto startup_info = CreateZeroed<STARTUPINFO>();
	auto process_info = CreateZeroed<PROCESS_INFORMATION>();
	auto command_line = CreateZeroed<TCHAR, 65536>();

	startup_info.cb = sizeof( STARTUPINFO );

	if( ( exec + " " + param ).length() + 1 > command_line.size() ) {
		std::cout << "ConsoleExecute error: Command line too long.\n";
		return false;
	}

	strncpy( command_line.data(), ( exec + " " + param ).c_str(), ( exec + " " + param ).length() + 1 );

	std::cout << "Executing: " << command_line.data() << "\n";

	if( !CreateProcess( nullptr, command_line.data(), nullptr, nullptr, FALSE, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &startup_info, &process_info ) ) {
		std::cout << "ConsoleExecute error: " << GetLastError() << "\n";
		return false;
	}

	execution_flag = true;

	std::thread( [process_info, &completion_flag, &execution_flag]() {
		WaitForSingleObject( process_info.hProcess, INFINITE );
		CloseHandle( process_info.hProcess );
		CloseHandle( process_info.hThread );

		completion_flag = true;

		std::cout << "Execution complete.\n";

		execution_flag = false;
	} ).detach();

	return true;
}

std::string SelectDirectory( const std::string& caption ) {
	std::string directory;

	CoInitialize( nullptr );

	auto dir_string = CreateZeroed<TCHAR, MAX_PATH>();
	auto browse_info = CreateZeroed<BROWSEINFO>();

	browse_info.pszDisplayName = dir_string.data();
	browse_info.lpszTitle = caption.c_str();
	browse_info.ulFlags = BIF_USENEWUI;
	browse_info.iImage = -1;

	auto item_id_list = SHBrowseForFolder( &browse_info );

	if( item_id_list ) {
		SHGetPathFromIDList( item_id_list, dir_string.data() );
		directory = dir_string.data();
	}

	CoUninitialize();

	std::replace( std::begin( directory ), std::end( directory ), '\\', '/' );

	return directory;
}

void RemoveDirectory( const std::string& directory ) {
	if( directory.empty() ) {
		return;
	}

	std::cout << "Removing " << directory << "\n";

	auto file_operation = CreateZeroed<SHFILEOPSTRUCT>();
	auto from = directory;

	if( from.back() == '/' ) {
		from.pop_back();
	}

	from += '\0';

	file_operation.wFunc = FO_DELETE;
	file_operation.pFrom = from.c_str();
	file_operation.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

	auto result = SHFileOperation( &file_operation );

	if( result ) {
		std::cout << "RemoveDirectory error: " << result << "\n";
	}

	std::cout << "Removed " << directory << "\n";
}

std::string GetWorkingDirectory() {
	auto dir = CreateZeroed<TCHAR, MAX_PATH>();

	GetCurrentDirectory( dir.size(), dir.data() );

	std::replace( std::begin( dir ), std::end( dir ), '\\', '/' );

	return dir.data();
}

void SetWorkingDirectory( const std::string& directory ) {
	SetCurrentDirectory( directory.c_str() );
}
