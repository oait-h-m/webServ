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

};

struct ServerConfig
{
	std::string host;
	int port;
	std::string	root;
	std::vector<std::string> server_names;
	std::map<int, std::string> error_pages;
	size_t client_max_body_size;
	std::vector<LocationConfig> locations;
};

struct WebServerConfig {
	std::vector<ServerConfig>	server_configs;
};


#endif /* INTERNAL_CONFIGS_HPP */
