#include "./ConfigParser.hpp"
#include <sstream>
#include <iostream>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

ConfigParser::ConfigParser(): _pos(0) {}

ConfigParser::ConfigParser(const ConfigParser &other) {}

ConfigParser::~ConfigParser() {}

ConfigParser&	ConfigParser::operator=(const ConfigParser &other) {
	/* This one should not be used, if anything changes, its going public
	 * and a new logic needs to be impelemented accordingly
	 */
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

void ConfigParser::parse() {
	Token	current;

	while (_peek().type != TOK_EOF) {
		current = _peek();
		if (current.word == "server")
			_parse_server();
		else
			throw(std::runtime_error("ConfigParser::parse(): Unknown global directive!"));
		current = _peek();
		if (current.type == TOK_EOF)
			return ;
	}
	return ;
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

void	ConfigParser::_parse_listen_directive(ServerConfig &sv_config) {
	std::string	arg;
	_consume();
	if (_peek().type != TOK_WORD)
		throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid listen directive args!"));
	arg = _peek().word;
	_validate_interface_port(arg, sv_config);
	_consume();
	_match(TOK_SEM);

}

void	ConfigParser::_parse_server_name_directive(ServerConfig &sv_config) {
	_consume();
	if (_peek().type != TOK_WORD)
		throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid  server_name directive args!"));
	if (_peek().word.size() > 255)
		throw(std::runtime_error("ConfigParser::_parse_server_directive(): Domain name is too long!"));
	sv_config.server_name = _peek().word;
	_consume();
	_match(TOK_SEM);
}

void	ConfigParser::_parse_cmb_size_directive(ServerConfig &sv_config) {
		std::stringstream	size_stream;
		long			size_container;
		std::string			arg;
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

void	ConfigParser::_parse_root_directive(ServerConfig &sv_config) {

	std::string root_dir;
	struct stat meta_data;
	_consume();
	if (_peek().type != TOK_WORD)
		throw(std::runtime_error("ConfigParser::_parse_server_directive(): Invalid root dir parameter"));
	root_dir = _peek().word;
	if (stat(root_dir.c_str(), &meta_data))
		throw(std::runtime_error("ConfigParser::_parse_server_directive(): Root directory path not found"));
	if (!S_ISDIR(meta_data.st_mode))
		throw(std::runtime_error("ConfigParser::_parse_server_directive(): Given path is not a directory"));
	sv_config.root = root_dir;
	_consume();
	_match(TOK_SEM);
}

void	ConfigParser::_parse_error_page_directive(ServerConfig &sv_config) {
	std::vector<std::string>	error_args(0);
	std::string	file_path;
	struct stat	meta_data;
	_consume();
	while (_peek().type != TOK_SEM) {
		if (_peek().type != TOK_WORD)
			throw(std::runtime_error("ConfigParser::_parse_error_page_directive(): Invalid error_page directive args"));
		error_args.push_back(_peek().word);
		_consume();
	}
	if (error_args.size() < 2)
		throw(std::runtime_error("ConfigParser::_parse_error_page_directive(): Invalid error_page number of args"));
	file_path = error_args[error_args.size() - 1];
	if (stat(file_path.c_str(), &meta_data) != 0) {
		throw std::runtime_error("ConfigParser::_parser_error_page_directive(): Error page file not found: " + file_path);
	}
	if (!S_ISREG(meta_data.st_mode)) {
		throw std::runtime_error("ConfigParser::_parse_error_page_directive(): Error page path is not a file: " + file_path);
	}
	if (access(file_path.c_str(), R_OK) != 0) {
		throw std::runtime_error("ConfigParser::_parse_error_page_directive(): Error page file is not readable: " + file_path);
	}
	for (size_t i = 0; i < error_args.size() - 1; i += 1) {
	std::stringstream enum_stream;
	long			enum_container;
		enum_stream.str(error_args[i]);
		enum_stream >> enum_container;
		if (enum_stream.fail() || !enum_stream.eof())
			throw(std::runtime_error("ConfigParser::_parse_error_page_directive(): Invalid enum number"));
		if (enum_container < 0 || enum_container > 699)
			throw(std::runtime_error("ConfigParser::_parse_error_page_directive(): enum number is out of range"));
		if (enum_container > 599)
			WARN("Error Number is a bit too large ! Try using a value less than 599");
		sv_config.error_pages[enum_container] = file_path;
	}
	_match(TOK_SEM);
}

void	ConfigParser::_parse_server_directive(ServerConfig &sv_config) {
	std::string	key_word = _peek().word;
	std::string arg;
	if (key_word == "listen")
		_parse_listen_directive(sv_config);
	else if (key_word == "server_name")
		_parse_server_name_directive(sv_config);
	else if (key_word == "client_max_body_size")
		_parse_cmb_size_directive(sv_config);
	else if (key_word == "root")
		_parse_root_directive(sv_config);
	else if (key_word == "error_page")
		_parse_error_page_directive(sv_config);
	else
		throw(std::runtime_error("ConfigParser::_parse_server_directive(): Unkown directive name: " + key_word));
}

WebServerConfig ConfigParser::generate_config(std::string &file_path) {
	Lexer	lexer(file_path);

	_tokens = lexer.tokenizer();
	parse();
	return (_global_config);
}
