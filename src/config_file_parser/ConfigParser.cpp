#include "./ConfigParser.hpp"
#include <sstream>
#include <iostream>
#include <arpa/inet.h>

ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser &other) {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const std::vector<Token> &tokens): _tokens(tokens), _pos(0) {}

ConfigParser&	ConfigParser::operator=(const ConfigParser &other) {
	return (*this);
}

const Token	&ConfigParser::_consume() {
	if (_pos < _tokens.size())
		_pos += 1;
	else
		throw(std::runtime_error("ConfigParser::_consume(): Unexpected end of tokens!"));
	return (_tokens[_pos - 1]);
}

const Token	&ConfigParser::_peek() const {
	if (_pos < _tokens.size())
		return (_tokens[_pos]);
	else
		throw(std::runtime_error("ConfigParser::_peek(): Out of Tokens range!"));
}

void	ConfigParser::_match(TokenType expected) {
	Token current;

	current = _peek();
	if (current.type == expected)
		_consume();
	else
		throw(std::runtime_error("ConfigParser::_match(): Unexpected Token Type !"));
}

const WebServerConfig	&ConfigParser::parse() {
	Token	current;

	while (_peek().type != TOK_EOF) {
		current = _peek();
		if (current.word == "server")
			_parse_server();
		else
			throw(std::runtime_error("ConfigParser::parse(): Unknown global directive!"));
		current = _peek();
		if (current.type == TOK_EOF)
			return (_global_config);
	}
	return (_global_config);
}

void	ConfigParser::_parse_server() {
	ServerConfig	sv_config;

	_match(TOK_WORD);
	_match(TOK_OPEN_BR);
	while (_peek().type != TOK_CLOSE_BR) {
		if (_peek().word == "location")
			_parse_location(sv_config);
		else
			_parse_server_directive(sv_config);
	}
	_match(TOK_CLOSE_BR);
	_global_config.server_configs.push_back(sv_config);
}

void	ConfigParser::_validate_interface_port(std::string &input, ServerConfig &sv_config) {
	std::stringstream stream(input);
	std::stringstream port_stream;
	std::string		host;
	std::string		port_str;
	long	port_container;
	struct in_addr	binary_addr;

	if (input.find(':') == std::string::npos) {
		host = "0.0.0.0";
		port_str = input;
	}
	else {
		std::getline(stream, host, ':');
		std::getline(stream, port_str, ':');
	}
	port_stream.str(port_str);
	port_stream >> port_container;
	if (port_stream.fail() || !port_stream.eof())
		throw(std::runtime_error("ConfigParser::_validate_interface_port(): Invalid port number"));
	if (port_container < 0 || port_container > 65535)
		throw(std::runtime_error("ConfigParser::_validate_interface_port(): port number is out of range"));
	if (port_container < 1024)
		WARN("Port Number is under 1024, Might need root permissions for it !");
	sv_config.port = static_cast<int>(port_container);
	if (host == "localhost")
		sv_config.host = "127.0.0.1";
	else {
		if (inet_pton(AF_INET, host.c_str(), &binary_addr) <= 0)
			throw(std::runtime_error("ConfigParser::_validate_interface_port(): Invalid host address " + host));
		sv_config.host = host;
	}
}

void	ConfigParser::_parse_server_directive(ServerConfig &sv_config) {
	std::string	key_word = _peek().word;
	int	port;
	if (key_word == "listen") {
		_consume();
		key_word = _peek().word;
		_validate_interface_port(key_word, sv_config);
		sv_config.port = port;
		_consume();
		_match(TOK_SEM);
	}
}
