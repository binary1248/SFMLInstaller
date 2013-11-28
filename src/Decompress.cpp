#include <miniz.c>
#include <iostream>
#include <Decompress.hpp>
#include <System.hpp>

bool DecompressArchive( const char* data, std::size_t size, const std::string& directory ) {
	auto zip_archive = CreateZeroed<mz_zip_archive>();

	if( !mz_zip_reader_init_mem( &zip_archive, data, size, 0 ) ) {
		std::cout << "mz_zip_reader_init_mem() failed!\n";
		return false;
	}

	for( unsigned int i = 0; i < mz_zip_reader_get_num_files( &zip_archive ); i++ ) {
		mz_zip_archive_file_stat file_stat;

		if( !mz_zip_reader_file_stat( &zip_archive, i, &file_stat ) ) {
			std::cout << "mz_zip_reader_file_stat() failed!\n";
			mz_zip_reader_end( &zip_archive );
			return false;
		}

		if( !mz_zip_reader_is_file_a_directory( &zip_archive, i ) ) {
			auto filename = directory + ( directory.empty() ? "" : "/" ) + file_stat.m_filename;

			if( !CreateDirectoryIfRequired( filename ) ) {
				std::cout << "DecompressArchive could not create directory.\n";
				return false;
			}

			if( !mz_zip_reader_extract_to_file( &zip_archive, i, filename.c_str(), 0 ) ) {
				std::cout << "mz_zip_reader_extract_to_file() failed!\n";
				mz_zip_reader_end( &zip_archive );
				return false;
			}
		}
	}

	if( !mz_zip_reader_end( &zip_archive ) ) {
		std::cout << "mz_zip_reader_end() failed!\n";
		return false;
	}

	return true;
}
