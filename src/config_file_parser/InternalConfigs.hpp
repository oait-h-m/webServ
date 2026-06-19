#ifndef INTERNAL_CONFIGS_HPP
# define INTERNAL_CONFIGS_HPP

#include "./Lexer.hpp"
#include <map>


struct LocationConfig {
	std::string path;
	std::string root;
	std::vector<std::string> index_files;
	std::vector<std::string> allowed_methods;
	bool autoindex;
	std::string upload_path;
	std::string cgi_extension;
	std::string cgi_path;
	std::string redirection;
	LocationConfig() {
		index_files.clear();
		allowed_methods.clear();
		autoindex = false;
	}

};

struct ServerConfig
{
	std::string host;
	int port;
	std::string	root;
	std::string  server_name;
	std::map<int, std::string> error_pages;
	size_t client_max_body_size;
	std::vector<LocationConfig> locations;
	ServerConfig() {
		port = 0;
		client_max_body_size = 0;
		error_pages.clear();
		locations.clear();
	}
};

struct WebServerConfig {
	std::vector<ServerConfig>	server_configs;
	WebServerConfig() { server_configs.clear(); }
};


#endif /* INTERNAL_CONFIGS_HPP */
