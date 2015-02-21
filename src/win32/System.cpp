#if !defined( _WIN32 )
#error This file should not be built on a non-Win32 system.
#endif

#include <System.hpp>
#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#include <iostream>
#include <algorithm>
#include <thread>

namespace sys {

bool create_directory_if_required( const std::string& directory ) {
	if( directory.length() > 247 ) {
		std::cout << "create_directory_if_required error: Path too long.\n";
	}

	auto pos = directory.find( '/' );

	while( pos != std::string::npos ) {
		std::string current_segment = directory.substr( 0, pos );

		if( !CreateDirectory( current_segment.c_str(), nullptr ) ) {
			auto error = GetLastError();

			if( error != ERROR_ALREADY_EXISTS ) {
				std::cout << "create_directory_if_required couldn't create directory (" << current_segment << "): " << error << "\n";
				return false;
			}
		}

		pos = directory.find( '/', pos + 1 );
	}

	return true;
}

bool try_execute( const std::string& executable, bool show ) {
	auto appended_executable = executable;

	if( ( appended_executable.find( ".exe" ) == std::string::npos ) && ( appended_executable.find( ".bat" ) == std::string::npos ) ) {
		appended_executable += ".exe";
	}

	auto result = reinterpret_cast<int>( ShellExecute( nullptr, "open", appended_executable.c_str(), "", nullptr, show ? SW_SHOW : SW_HIDE ) );

	std::cout << "Trying to execute " << appended_executable << ": " << result << "\n";

	if( result > 32 ) {
		return true;
	}

	return false;
}

std::string get_path_from_user( const std::vector<std::string>& executables ) {
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

	auto filename = create_zeroed<TCHAR, 65536>();
	auto open_filename = create_zeroed<OPENFILENAME>();

	open_filename.lStructSize = sizeof( OPENFILENAME );
	open_filename.lpstrFilter = strFilter.c_str();
	open_filename.lpstrFile = filename.data();
	open_filename.nMaxFile = filename.size();
	open_filename.lpstrTitle = strTitle.c_str();
	open_filename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	auto working_directory = get_working_directory();

	auto result = GetOpenFileName( &open_filename );

	set_working_directory( working_directory );

	if( !result ) {
		return "";
	}

	return std::string( filename.data() );
}

bool console_execute( const std::string& exec, const std::string& param, std::atomic_bool& completion_flag, std::atomic_bool& execution_flag ) {
	auto startup_info = create_zeroed<STARTUPINFO>();
	auto process_info = create_zeroed<PROCESS_INFORMATION>();
	auto command_line = create_zeroed<TCHAR, 65536>();

	startup_info.cb = sizeof( STARTUPINFO );

	if( ( exec + " " + param ).length() + 1 > command_line.size() ) {
		std::cout << "console_execute error: Command line too long.\n";
		return false;
	}

	strncpy( command_line.data(), ( exec + " " + param ).c_str(), ( exec + " " + param ).length() + 1 );

	std::cout << "Executing: " << command_line.data() << "\n";

	if( !CreateProcess( nullptr, command_line.data(), nullptr, nullptr, FALSE, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &startup_info, &process_info ) ) {
		std::cout << "console_execute error: " << GetLastError() << "\n";
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

std::string select_directory( const std::string& caption ) {
	std::string directory;

	CoInitialize( nullptr );

	auto dir_string = create_zeroed<TCHAR, MAX_PATH>();
	auto browse_info = create_zeroed<BROWSEINFO>();

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

void remove_directory( const std::string& directory ) {
	if( directory.empty() ) {
		return;
	}

	std::cout << "Removing " << directory << "\n";

	auto file_operation = create_zeroed<SHFILEOPSTRUCT>();
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
		std::cout << "remove_directory error: " << result << "\n";
	}

	std::cout << "Removed " << directory << "\n";
}

std::string get_working_directory() {
	auto dir = create_zeroed<TCHAR, MAX_PATH>();

	GetCurrentDirectory( dir.size(), dir.data() );

	std::replace( std::begin( dir ), std::end( dir ), '\\', '/' );

	return dir.data();
}

void set_working_directory( const std::string& directory ) {
	SetCurrentDirectory( directory.c_str() );
}

}
