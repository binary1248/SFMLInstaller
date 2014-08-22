#include <atomic>
#include <SFMLInstaller.hpp>
#include <System.hpp>

void SFMLInstaller::Process() {
////////////////////////////////////////////////////////////////////////////////
// Get SFML source
////////////////////////////////////////////////////////////////////////////////

	static bool sfml_archive_downloaded = false;
	static std::string sfml_archive;

	// Download SFML source archive.
	if( !sfml_archive_downloaded ) {
		std::cout << "Downloading SFML...\n";
		Download( "codeload.github.com", 443, "/LaurentGomila/SFML/zip/master", "Downloading SFML source archive...", sfml_archive, sfml_archive_downloaded );

		return;
	}

	static bool sfml_archive_decompressed = false;

	// Decompress SFML source archive.
	if( !sfml_archive_decompressed ) {
		std::cout << "Decompressing SFML...\n";
		Decompress( sfml_archive, "", "Decompressing SFML source archive...", sfml_archive_decompressed );

		return;
	}

	static std::string install_location;

	if( install_location.empty() ) {
		install_location = SelectDirectory( "Where do you want to install SFML to?" );

		if( !install_location.empty() ) {
			std::cout << "Installing to " << install_location << "\n";
		}

		return;
	}

////////////////////////////////////////////////////////////////////////////////
// Adjust directories
////////////////////////////////////////////////////////////////////////////////

	static bool directory_adjusted = false;

	static std::string working_directory;

	// Adjust working directory.
	if( !directory_adjusted ) {
		std::cout << "Adjusting current directory...\n";

		working_directory = GetWorkingDirectory();

		std::cout << "Working directory: " << working_directory << "\n";

		SetWorkingDirectory( working_directory + "/SFML-master" );

		directory_adjusted = true;
	}

////////////////////////////////////////////////////////////////////////////////
// Configuration
////////////////////////////////////////////////////////////////////////////////

	const static std::string default_settings =
		"-D SFML_BUILD_EXAMPLES:BOOL=TRUE "
		"-D SFML_USE_STATIC_STD_LIBS:BOOL=FALSE "
		"-D SFML_BUILD_DOC:BOOL=FALSE "
		"-D CMAKE_VERBOSE_MAKEFILE:BOOL=TRUE "
	;

	const static std::string static_settings = "-D BUILD_SHARED_LIBS:BOOL=FALSE ";
	const static std::string dynamic_settings = "-D BUILD_SHARED_LIBS:BOOL=TRUE ";
	const static std::string debug_settings = "-D CMAKE_BUILD_TYPE:STRING=Debug ";
	const static std::string release_settings = "-D CMAKE_BUILD_TYPE:STRING=Release ";

	std::string install_prefix = "-D CMAKE_INSTALL_PREFIX:PATH=\"" + install_location + "\" ";
	std::string settings = default_settings + install_prefix;

////////////////////////////////////////////////////////////////////////////////
// Dynamic Debug
////////////////////////////////////////////////////////////////////////////////

	static std::atomic_bool configured_dynamic_debug{ false };

	// Run CMake to generate dynamic debug configuration.
	if( !configured_dynamic_debug ) {
		Configure( "dynamic debug", settings + dynamic_settings + debug_settings, configured_dynamic_debug );
		return;
	}

	static std::atomic_bool built_dynamic_debug{ false };

	// Make dynamic debug.
	if( !built_dynamic_debug ) {
		Build( "dynamic debug", built_dynamic_debug );
		return;
	}

////////////////////////////////////////////////////////////////////////////////
// Dynamic Release
////////////////////////////////////////////////////////////////////////////////

	static std::atomic_bool configured_dynamic_release{ false };

	// Run CMake to generate dynamic release configuration.
	if( !configured_dynamic_release ) {
		Configure( "dynamic release", settings + dynamic_settings + release_settings, configured_dynamic_release );
		return;
	}

	static std::atomic_bool built_dynamic_release{ false };

	// Make dynamic release.
	if( !built_dynamic_release ) {
		Build( "dynamic release", built_dynamic_release );
		return;
	}

////////////////////////////////////////////////////////////////////////////////
// Static Debug
////////////////////////////////////////////////////////////////////////////////

	static std::atomic_bool configured_static_debug{ false };

	// Run CMake to generate static debug configuration.
	if( !configured_static_debug ) {
		Configure( "static debug", settings + static_settings + debug_settings, configured_static_debug );
		return;
	}

	static std::atomic_bool built_static_debug{ false };

	// Make static debug.
	if( !built_static_debug ) {
		Build( "static debug", built_static_debug );
		return;
	}

////////////////////////////////////////////////////////////////////////////////
// Static Release
////////////////////////////////////////////////////////////////////////////////

	static std::atomic_bool configured_static_release{ false };

	// Run CMake to generate static release configuration.
	if( !configured_static_release ) {
		Configure( "static release", settings + static_settings + release_settings, configured_static_release );
		return;
	}

	static std::atomic_bool built_static_release{ false };

	// Make static release.
	if( !built_static_release ) {
		Build( "static release", built_static_release );
		return;
	}

////////////////////////////////////////////////////////////////////////////////
// Remove temporaries
////////////////////////////////////////////////////////////////////////////////

	std::cout << "Removing temporary directories.\n";

	std::cout << "Working directory: " << working_directory << "\n";

	SetWorkingDirectory( working_directory );

	RemoveDirectory( working_directory + "/SFML-master" );

	std::cout << "Removed temporary directories.\n";

////////////////////////////////////////////////////////////////////////////////
// Complete
////////////////////////////////////////////////////////////////////////////////

	std::cout << "Showing completion message.\n";

	Complete( "SFML installation is complete. You might want to create the environment variable SFML_ROOT and set it to\n" + install_location + " for convenience. Happy coding." );
}
