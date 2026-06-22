#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP
# define WARN(string) \
	std::cout << "\033[33m" << "[WARNING] " <<\
	string << "\033[0m" << std::endl

#include "./InternalConfigs.hpp"
#include "./Lexer.hpp"
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/stat.h>

class ConfigParser {
	private:
		std::vector<Token>	_tokens;
		size_t				_pos;
		WebServerConfig		_global_config;

		// Main entry points for location and server parsers.
		void	_parse_server();
		void	_parse_location(ServerConfig &server);
		void	_parse_server_directive(ServerConfig &sv_config);
		void	_parse_location_directive(LocationConfig &lc_config);

		// Server Config directive dispatchers
		void	_parse_listen_directive(ServerConfig &sv_config);
		void	_parse_server_name_directive(ServerConfig &sv_config);

		// Location config directive dispatchers
		void	_parse_allowed_methods(LocationConfig &lc_cnof);
		void	_parse_autoindex(LocationConfig &lc_conf);
		void	_parse_index(LocationConfig &lc_conf);
		void	_parse_return(LocationConfig &lc_conf);
		void	_parse_upload_store(LocationConfig &lc_conf);
		void	_parse_cgi_pass(LocationConfig &lc_conf);

		// Helper methods for walking around or performing parameters checks.
		void	_validate_interface_port(std::string &input, ServerConfig &sv_config);
		const	Token	&_consume();
		const	Token	&_peek() const;
		void	_match(TokenType expected);
		ConfigParser(const ConfigParser &other);
		ConfigParser&	operator=(const ConfigParser &other);

		/* Template methods */
		template <typename T>
			void	_parse_cmb_size_directive(T &conf) {
				std::stringstream	size_stream;
				long			size_container;
				std::string			arg;
				_consume();
				if (_peek().type != TOK_WORD)
					throw(std::runtime_error("ConfigParser::_parse_directive(): \
								Invalid client_max_body_size directive args!"));
				arg = _peek().word;
				size_stream.str(arg);
				size_stream >> size_container;
				if (size_stream.fail() || !size_stream.eof())
					throw(std::runtime_error("ConfigParser::_parse_cmb_size_directive(): \
								Invalid size value: Non numerical"));
				if (size_container < 0)
					throw(std::runtime_error("ConfigParser::_parse_cmb_size_directive(): \
								Invalid size number: Not positive"));
				conf.client_max_body_size = static_cast<size_t>(size_container);
				_consume();
				_match(TOK_SEM);
			}
		template <typename T>
			void	_parse_root_directive(T &conf) {
				std::string root_dir;
				struct stat meta_data;
				_consume();
				if (_peek().type != TOK_WORD)
					throw(std::runtime_error("ConfigParser::_parse_root_directive(): \
								Invalid root dir parameter"));
				root_dir = _peek().word;
				if (stat(root_dir.c_str(), &meta_data))
					throw(std::runtime_error("ConfigParser::_parse_root_directive(): \
								Root directory path not found"));
				if (!S_ISDIR(meta_data.st_mode))
					throw(std::runtime_error("ConfigParser::_parse_root_directive(): \
								Given path is not a directory"));
				conf.root = root_dir;
				_consume();
				_match(TOK_SEM);
			}
		template <typename T>
			void	_parse_error_page_directive(T &conf) {
				std::vector<std::string>	error_args(0);
				std::string	file_path;
				struct stat	meta_data;
				_consume();
				while (_peek().type != TOK_SEM) {
					if (_peek().type != TOK_WORD)
						throw(std::runtime_error("ConfigParser::_parse_error_page_directive(): \
									Invalid error_page directive args"));
					error_args.push_back(_peek().word);
					_consume();
				}
				if (error_args.size() < 2)
					throw(std::runtime_error("ConfigParser::_parse_error_page_directive(): \
								Invalid error_page number of args"));
				file_path = error_args[error_args.size() - 1];
				if (stat(file_path.c_str(), &meta_data) != 0) {
					throw std::runtime_error("ConfigParser::_parser_error_page_directive(): \
							Error page file not found: " + file_path);
				}
				if (!S_ISREG(meta_data.st_mode)) {
					throw std::runtime_error("ConfigParser::_parse_error_page_directive(): \
							Error page path is not a file: " + file_path);
				}
				if (access(file_path.c_str(), R_OK) != 0) {
					throw std::runtime_error("ConfigParser::_parse_error_page_directive(): \
							Error page file is not readable: " + file_path);
				}
				for (size_t i = 0; i < error_args.size() - 1; i += 1) {
					std::stringstream enum_stream;
					long			enum_container;
					enum_stream.str(error_args[i]);
					enum_stream >> enum_container;
					if (enum_stream.fail() || !enum_stream.eof())
						throw(std::runtime_error("ConfigParser::_parse_error_page_directive(): \
									Invalid enum number"));
					if (enum_container < 0 || enum_container > 699)
						throw(std::runtime_error("ConfigParser::_parse_error_page_directive(): \
									enum number is out of range"));
					if (enum_container > 599)
						WARN("Error Number is a bit too large ! \
								Try using a value less than 599");
					conf.error_pages[enum_container] = file_path;
				}
				_match(TOK_SEM);
			}
	public:
		ConfigParser();
		void	parse();
		WebServerConfig generate_config(std::string &file_path);
		~ConfigParser();
};


#endif
