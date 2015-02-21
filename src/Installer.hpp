#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Window.hpp>
#include <atomic>
#include <memory>

namespace sfn {

class HTTPClient;

}

class Installer {
public:
	~Installer();

	void run();

protected:
	Installer();

	virtual void process() = 0;
	void complete( const std::string& caption );

	void download( const std::string& host, unsigned short port, const std::string& uri, const std::string& caption, std::string& file, bool& flag );
	void decompress( const std::string& file, const std::string& directory, const std::string& caption, bool& flag );

	void configure( const std::string& label, const std::string& settings, std::atomic_bool& flag );
	void build( const std::string& label, std::atomic_bool& flag );

private:
	void Center( sfg::Widget::Ptr widget );

	bool EnvironmentSetup();
	bool ProcessActive() const;

	void GetCMake();
	void GetCompiler();
	void ShowLogWindow();
	void SetLogCaption( const std::string& caption );

	sf::Window m_app_window{ sf::VideoMode( 600, 400 ), "SFML Installer", sf::Style::Titlebar | sf::Style::Close };
	sfg::SFGUI m_sfgui;
	sfg::Desktop m_desktop;

	std::unique_ptr<sfn::HTTPClient> m_http_client;

	const static std::string m_cmake_version;

	std::string m_working_directory;
	std::string m_cmake_path;
	std::string m_cmake_archive;
	std::string m_vsvars_exec;
	std::string m_compiler_exec;

	std::atomic_bool m_processing{ false };

	int m_compiler = 0;
	bool m_getting_compiler = false;
	bool m_compiler_ready = false;

	bool m_getting_cmake = false;
	bool m_cmake_standalone = false;
	bool m_cmake_downloaded = false;
	bool m_cmake_decompressed = false;
	bool m_cmake_ready = false;

	std::map<std::string, std::pair<std::function<void()>, bool>> m_update_callbacks;
};
