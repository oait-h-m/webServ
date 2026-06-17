#include "./ConfigParser.hpp"
#include <sstream>
#include <iostream>
#include <arpa/inet.h>
#include <sys/stat.h>

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
	std::string arg;
	if (key_word == "listen") {
		_consume();
		if (_peek().type != TOK_WORD)
			throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid listen directive args!"));
		arg = _peek().word;
		_validate_interface_port(arg, sv_config);
		_consume();
		_match(TOK_SEM);
	}
	else if (key_word == "server_name") {
		_consume();
		if (_peek().type != TOK_WORD)
			throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid  server_name directive args!"));
		while (_peek().type != TOK_SEM) {
			if (_peek().word.size() > 255)
				throw(std::runtime_error("ConfigParser::_parse_server_directive(): Domain name is too long!"));
			sv_config.server_names.push_back(_peek().word);
			_consume();
		}
		_match(TOK_SEM);
	}
	else if (key_word == "client_max_body_size") {
		std::stringstream	size_stream;
		long long			size_container;
		_consume();
		if (_peek().type != TOK_WORD)
			throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid client_max_body_size directive args!"));
		arg = _peek().word;
		size_stream.str(arg);
		size_stream >> size_container;
		if (size_stream.fail() || !size_stream.eof())
			throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid size value: Non numerical"));
		if (size_container < 0)
			throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid size number: Not positive"));
		sv_config.client_max_body_size = static_cast<size_t>(size_container);
		_consume();
		_match(TOK_SEM);
	}
	else if (key_word == "error_page") {
		std::stringstream	number_stream;
		long long			number_container;
		std::vector<int>	error_nums(0);
		_consume();
		while (_peek().type != TOK_SEM) {
			if (_peek().type != TOK_WORD)
				throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid error_page directive args"));
			number_stream.str(arg);
			number_stream >> number_container;
			if (number_stream.fail() || !number_stream.eof())
				throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid number value: Non numerical"));
			if (number_container < 0 || number_container > 1000)
				throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid error number!"));
			error_nums.push_back(number_container);
			_consume();
		}
		//TODO: Handle the last argument which is the error page
		_match(TOK_SEM);
	}
	else if (key_word == "root") {
		std::string root_dir;
		struct stat meta_data;
		_consume();
		_match(TOK_WORD);
		root_dir = _peek().word;
		if (stat(root_dir.c_str(), &meta_data))
			throw(std::runtime_error("ConfigParser::_parse_server_directive(): Root directory not found"));
		sv_config.root = root_dir;
	}
	else
		throw(std::runtime_error("ConfigParser::_parse_server_directive(): Unkown directive name: " + key_word));
}
