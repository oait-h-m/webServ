#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# define WARN(string) \
	std::cout << "\033[33m" << "[WARNING] " <<\
	string << "\033[0m" << std::endl

#include "./InternalConfigs.hpp"
#include "./Lexer.hpp"

class ConfigParser {
	private:
		std::vector<Token>	_tokens;
		size_t				_pos;
		WebServerConfig		_global_config;

		void	_parse_server();
		void	_parse_listen_directive(ServerConfig &sv_config);
		void	_parse_server_name_directive(ServerConfig &sv_config);
		void	_parse_cmb_size_directive(ServerConfig &sv_config);
		void	_parse_root_directive(ServerConfig &sv_config);
		void	_parse_error_page_directive(ServerConfig &sv_config);
		void	_parse_location(ServerConfig &server);
		void	_parse_location_directive(LocationConfig &lc_config);
		void	_validate_interface_port(std::string &input, ServerConfig &sv_config);
		void	_parse_server_directive(ServerConfig &sv_config);
		const	Token	&_consume();
		const	Token	&_peek() const;
		void	_match(TokenType expected);
		ConfigParser();
		ConfigParser(const ConfigParser &other);
		ConfigParser&	operator=(const ConfigParser &other);
	public:
		ConfigParser(const std::vector<Token> &tokens);
		void	parse();
		WebServerConfig generate_config(std::string &file_path);
		~ConfigParser();


};


#endif
