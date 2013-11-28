#include <iostream>
#include <string>
#include <Installer.hpp>
#include <System.hpp>
#include <Decompress.hpp>
#include <CMake.hpp>
#include <Make.hpp>
#include <NMake.hpp>

Installer::Installer() {
	sfn::Start();

	m_http_client.LoadCertificate( "codeload.github.com", sfn::TlsCertificate::Create(
		"-----BEGIN CERTIFICATE-----\r\n"
		"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\r\n"
		"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"
		"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\r\n"
		"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\r\n"
		"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\r\n"
		"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\r\n"
		"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\r\n"
		"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\r\n"
		"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\r\n"
		"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\r\n"
		"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\r\n"
		"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\r\n"
		"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\r\n"
		"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\r\n"
		"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\r\n"
		"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\r\n"
		"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\r\n"
		"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\r\n"
		"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\r\n"
		"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\r\n"
		"+OkuE6N36B9K\r\n"
		"-----END CERTIFICATE-----\r\n"
	) );

	m_working_directory = GetWorkingDirectory();
}

Installer::~Installer() {
	SetWorkingDirectory( m_working_directory );

	if( m_cmake_standalone ) {
		RemoveDirectory( m_working_directory + "/" + m_cmake_version );
	}

	sfn::Stop();
}

void Installer::Run() {
	m_app_window.setFramerateLimit( 60 );
	m_app_window.setActive();

	glViewport( 0, 0, m_app_window.getSize().x, m_app_window.getSize().y );

	sf::Clock clock;
	sf::Event event;

	while( m_app_window.isOpen() ) {
		while( m_app_window.pollEvent( event ) ) {
			if( event.type == sf::Event::Closed ) {
				m_app_window.close();
			}
			else {
				m_desktop.HandleEvent( event );
			}
		}

		m_http_client.Update();
		m_desktop.Update( clock.restart().asSeconds() );

		for( auto iter = std::begin( m_update_callbacks ); iter != std::end( m_update_callbacks ); ) {
			if( iter->second.second ) {
				iter->second.first();
				++iter;
			}
			else {
				iter = m_update_callbacks.erase( iter );
			}
		}

		if( !ProcessActive() ) {
			if( EnvironmentSetup() ) {
				Process();
			}
		}

		glClear( GL_COLOR_BUFFER_BIT );
		m_sfgui.Display( m_app_window );
		m_app_window.display();
	}
}

void Installer::Center( sfg::Widget::Ptr widget ) {
	m_desktop.Update( 0.f );

	widget->SetPosition(
		{
			( static_cast<float>( m_app_window.getSize().x ) - widget->GetAllocation().width ) / 2.f,
			( static_cast<float>( m_app_window.getSize().y ) - widget->GetAllocation().height ) / 2.f
		}
	);
}

void Installer::Download( const std::string& host, unsigned short port, const std::string& uri, const std::string& caption, std::string& file, bool& flag ) {
	m_processing = true;

	auto window = sfg::Window::Create( sfg::Window::BACKGROUND );
	window->SetId( "Download" );

	m_desktop.Add( window );

	auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.f );
	window->Add( box );
	box->Pack( sfg::Label::Create( caption ) );

	auto download_status = sfg::ProgressBar::Create();
	download_status->SetRequisition( { 400.f, 20.f } );
	box->Pack( download_status );

	auto download_status_text = sfg::Label::Create( "" );
	box->Pack( download_status_text );

	Center( window );

	sfn::HTTPRequest request{};
	request.SetMethod( "GET" );
	request.SetHeaderValue( "Host", host );
	request.SetURI( uri );
	m_http_client.SendRequest( request, host, port, port == 443 );

	m_update_callbacks[ "Download" ] = std::make_pair( [=, &file, &flag]() {
		auto response = m_http_client.GetResponse( request, host, port );

		if( response.IsHeaderComplete() ) {
			auto content_length_str = response.GetHeaderValue( "Content-Length" );

			if( !content_length_str.empty() ) {
				auto content_length = std::stoi( content_length_str );

				if( content_length ) {
					std::string status = "Received " + std::to_string( response.GetBody().length() ) + " out of " + std::to_string( content_length ) + " bytes.";

					download_status->SetFraction( static_cast<float>( response.GetBody().length() ) / static_cast<float>( content_length ) );
					download_status_text->SetText( status );
				}
			}
			else {
				m_http_client = sfn::HTTPClient{};
				m_http_client.SendRequest( request, host, port, port == 443 );
			}
		}

		if( response.IsComplete() ) {
			m_desktop.Remove( sfg::Widget::GetWidgetById( "Download" ) );
			m_update_callbacks[ "Download" ].second = false;
			m_processing = false;

			flag = true;

			file = std::move( response.GetBody() );
		}
	}, true );
}

void Installer::Decompress( const std::string& file, const std::string& directory, const std::string& caption, bool& flag ) {
	m_processing = true;

	auto window = sfg::Window::Create( sfg::Window::BACKGROUND );
	window->SetId( "Decompress" );

	m_desktop.Add( window );

	auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.f );
	window->Add( box );
	box->Pack( sfg::Label::Create( caption ) );

	Center( window );

	m_update_callbacks[ "Decompress" ] = std::make_pair( [=, &file, &flag]() {
		if( DecompressArchive( file.c_str(), file.length(), directory ) ) {
			m_desktop.Remove( sfg::Widget::GetWidgetById( "Decompress" ) );
			m_update_callbacks[ "Decompress" ].second = false;
			m_processing = false;

			flag = true;
		}
		else {
			m_app_window.close();
		}
	}, true );
}

void Installer::GetCMake() {
	m_cmake_path = GetCMakePath( true );

	if( !m_cmake_path.empty() ) {
		m_cmake_path = std::string( "\"" ) + m_cmake_path + "\"";
		m_cmake_ready = true;
		return;
	}

	m_processing = true;

	auto window = sfg::Window::Create( sfg::Window::BACKGROUND );
	window->SetId( "GetCMake" );

	m_desktop.Add( window );

	auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.f );
	window->Add( box );
	box->Pack( sfg::Label::Create( "Could not detect any CMake installation.\nIs CMake installed?" ) );

	auto cmake_yes_no_button_box = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 20.f );

	auto cmake_installed_yes = sfg::Button::Create( "Yes" );
	cmake_installed_yes->SetRequisition( { 40.f, 20.f } );
	cmake_yes_no_button_box->Pack( cmake_installed_yes );

	auto cmake_installed_no = sfg::Button::Create( "No" );
	cmake_installed_no->SetRequisition( { 40.f, 20.f } );
	cmake_yes_no_button_box->Pack( cmake_installed_no );

	box->Pack( cmake_yes_no_button_box );

	cmake_installed_yes->GetSignal( sfg::Widget::OnLeftClick ).Connect( [=]() {
		m_cmake_path = GetCMakePath( false );
	} );

	cmake_installed_no->GetSignal( sfg::Widget::OnLeftClick ).Connect( [=]() {
		auto current_directory = GetWorkingDirectory();

		m_cmake_path = current_directory + "/" + m_cmake_version + "/bin/cmake";
		m_cmake_standalone = true;
	} );

	Center( window );

	m_update_callbacks[ "GetCMake" ] = std::make_pair( [=]() {
		if( !m_cmake_path.empty() ) {
			m_desktop.Remove( sfg::Widget::GetWidgetById( "GetCMake" ) );
			m_update_callbacks[ "GetCMake" ].second = false;
			m_processing = false;

			m_cmake_path = std::string( "\"" ) + m_cmake_path + "\"";

			if( m_cmake_standalone && m_cmake_archive.empty() && !ProcessActive() ) {
				std::cout << "Downloading standalone CMake.\n";

				Download( "www.cmake.org", 80, "/files/v2.8/" + m_cmake_version + ".zip", "Downloading standalone CMake archive...", m_cmake_archive, m_cmake_downloaded );

				m_update_callbacks[ "DownloadStandaloneCMake" ] = std::make_pair( [=]() {
					if( m_cmake_downloaded && !m_cmake_decompressed && !ProcessActive() ) {
						m_update_callbacks[ "DownloadStandaloneCMake" ].second = false;

						std::cout << "Standalone CMake download complete.\n";
						std::cout << "Decompressing standalone CMake.\n";

						Decompress( m_cmake_archive, "", "Decompressing standalone CMake archive...", m_cmake_decompressed );

						m_update_callbacks[ "DecompressStandaloneCMake" ] = std::make_pair( [=]() {
							if( m_cmake_decompressed ) {
								m_update_callbacks[ "DecompressStandaloneCMake" ].second = false;

								std::cout << "Standalone CMake decompress complete.\n";

								m_cmake_ready = true;
							}
						}, true );
					}
				}, true );
			}
			else {
				std::cout << "Using existing CMake: " << m_cmake_path << "\n";
				m_cmake_ready = true;
			}
		}
	}, true );
}

void Installer::GetCompiler() {
	m_processing = true;

	auto window = sfg::Window::Create( sfg::Window::BACKGROUND );
	window->SetId( "GetCompiler" );

	m_desktop.Add( window );

	auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.f );
	window->Add( box );
	box->Pack( sfg::Label::Create( "Which do you use for development?" ) );

	auto codeblocks_mingw_radio = sfg::RadioButton::Create( "Code::Blocks / MinGW" );
	box->Pack( codeblocks_mingw_radio );
	auto msvc_nmake_radio = sfg::RadioButton::Create( "Visual Studio / NMake", codeblocks_mingw_radio->GetGroup() );
	box->Pack( msvc_nmake_radio );

	auto compiler_button_next = sfg::Button::Create( "Next" );
	box->Pack( compiler_button_next );

	compiler_button_next->GetSignal( sfg::Widget::OnLeftClick ).Connect( [=]() {
		if( codeblocks_mingw_radio->IsActive() ) {
			m_compiler_exec = GetMakePath();

			if( !m_compiler_exec.empty() ) {
				m_compiler_exec = std::string( "\"" ) + m_compiler_exec + "\"";
				m_compiler = 1;
			}
		}
		else if( msvc_nmake_radio->IsActive() ) {
			m_vsvars_exec = GetNMakePath();

			if( !m_vsvars_exec.empty() ) {
				m_vsvars_exec = std::string( "\"" ) + m_vsvars_exec + "\" && ";
				m_compiler_exec = "nmake";
				m_compiler = 2;
			}
		}
		else {
			m_compiler_exec = "";
			m_compiler = 0;
		}
	} );

	Center( window );

	m_update_callbacks[ "GetCompiler" ] = std::make_pair( [=]() {
		if( m_compiler && !m_compiler_exec.empty() ) {
			m_desktop.Remove( sfg::Widget::GetWidgetById( "GetCompiler" ) );
			m_update_callbacks[ "GetCompiler" ].second = false;
			m_processing = false;

			m_compiler_ready = true;
		}
	}, true );
}

void Installer::ShowLogWindow() {
	if( sfg::Widget::GetWidgetById( "Log" ) ) {
		return;
	}

	auto window = sfg::Window::Create( sfg::Window::BACKGROUND );
	window->SetId( "Log" );

	m_desktop.Add( window );

	auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.f );
	window->Add( box );
	auto log_caption = sfg::Label::Create( " " );
	log_caption->SetId( "LogCaption" );
	box->Pack( log_caption );

	Center( window );

	m_update_callbacks[ "LogWindow" ] = std::make_pair( [=]() {
		if( !m_processing ) {
			m_desktop.Remove( sfg::Widget::GetWidgetById( "Log" ) );
			m_update_callbacks[ "LogWindow" ].second = false;

			m_desktop.Update( 0.f );
		}
	}, true );
}

void Installer::SetLogCaption( const std::string& caption ) {
	auto log_caption = sfg::Widget::GetWidgetById( "LogCaption" );

	if( !log_caption ) {
		return;
	}

	auto label = std::dynamic_pointer_cast<sfg::Label>( log_caption );

	if( label ) {
		label->SetText( caption );
	}

	auto log_window = sfg::Widget::GetWidgetById( "Log" );

	if( !log_window ) {
		return;
	}

	auto window = std::dynamic_pointer_cast<sfg::Window>( log_window );

	if( window ) {
		Center( window );
	}
}

void Installer::Complete( const std::string& caption ) {
	m_processing = true;

	auto window = sfg::Window::Create( sfg::Window::BACKGROUND );

	m_desktop.Add( window );

	auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.f );
	window->Add( box );
	auto label1 = sfg::Label::Create( caption );
	label1->SetRequisition( sf::Vector2f( 300.f, 0.f ) );
	label1->SetLineWrap( true );
	box->Pack( label1 );

	auto complete_button = sfg::Button::Create( "Close" );
	complete_button->SetRequisition( { 40.f, 20.f } );
	box->Pack( complete_button );

	complete_button->GetSignal( sfg::Widget::OnLeftClick ).Connect( [=]() {
		m_app_window.close();
	} );

	Center( window );

	SetWorkingDirectory( m_working_directory );

	if( m_cmake_standalone ) {
		RemoveDirectory( m_working_directory + "/" + m_cmake_version );

		m_cmake_standalone = false;
	}
}

void Installer::Configure( const std::string& label, const std::string& settings, std::atomic_bool& flag ) {
	remove( "CMakeCache.txt" );

	std::string command_line = ( ( m_compiler == 1 ) ? "-G \"MinGW Makefiles\" " : "-G \"NMake Makefiles\" " );

	command_line += settings;
	command_line += " .";

	auto result = ConsoleExecute( m_vsvars_exec + m_cmake_path, command_line, flag, m_processing );

	if( !result ) {
		m_app_window.close();
	}

	ShowLogWindow();

	auto caption = std::string( "Generating " ) + label + " configuration...";
	std::cout << caption << "\n";
	SetLogCaption( caption );
}

void Installer::Build( const std::string& label, std::atomic_bool& flag ) {
	auto result = ConsoleExecute( m_vsvars_exec + m_compiler_exec, "install", flag, m_processing );

	if( !result ) {
		m_app_window.close();
	}

	ShowLogWindow();

	auto caption = std::string( "Building " ) + label + " configuration...";
	std::cout << caption << "\n";
	SetLogCaption( caption );
}

bool Installer::EnvironmentSetup() {
	// Choose compiler.
	if( !m_compiler_ready ) {
		std::cout << "Choosing compiler...\n";
		GetCompiler();

		return false;
	}

	// Get CMake.
	if( !m_cmake_ready ) {
		std::cout << "Getting CMake...\n";
		GetCMake();

		return false;
	}

	return true;
}

bool Installer::ProcessActive() const {
	return m_processing;
}
