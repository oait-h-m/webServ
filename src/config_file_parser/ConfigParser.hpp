#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

#include "./InternalConfigs.hpp"
#include "./Lexer.hpp"

class ConfigParser {
	private:
		std::vector<Token>	_tokens;
		size_t				_pos;
		WebServerConfig		_global_config;

		void	_parse_server();
		void	_parse_location(ServerConfig &server);
		void	_parse_location_directive(LocationConfig &lc_config);
		void	_parse_server_directive(ServerConfig &sv_config);
		const	Token	&_consume();
		const	Token	&_peek() const;
		void	_match(TokenType expected);
		ConfigParser();
		ConfigParser(const ConfigParser &other);
		ConfigParser&	operator=(const ConfigParser &other);
	public:
		ConfigParser(const std::vector<Token> &tokens);
		const WebServerConfig	&parse();
		~ConfigParser();


};


#endif
