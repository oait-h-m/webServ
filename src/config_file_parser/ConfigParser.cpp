#include "./ConfigParser.hpp"
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

void	ConfigParser::_parse_location(ServerConfig &sv_config) {
	LocationConfig	loc_conf;
	std::string		path;

	_match(TOK_WORD);
	if (_peek().type != TOK_WORD)
		throw(std::runtime_error("ConfigParser::_parse_location(): Invalid token type"));
	loc_conf.path = _peek().word;
	_consume();
	loc_conf.root = sv_config.root;
	loc_conf.client_max_body_size = sv_config.client_max_body_size;
	loc_conf.error_pages = sv_config.error_pages;
	
	_match(TOK_OPEN_BR);
	while (_peek().type != TOK_CLOSE_BR)
		_parse_location_directive(loc_conf);
	_match(TOK_CLOSE_BR);
	sv_config.locations.push_back(loc_conf);
}

void	ConfigParser::_parse_location_directive(LocationConfig &lc_conf) {
	std::string	key_word = _peek().word;
	std::string arg;
	if (key_word == "allowed_methods")
		_parse_allowed_methods(lc_conf);
	else if (key_word == "autoindex")
		_parse_autoindex(lc_conf);
	else if (key_word == "index")
		_parse_index(lc_conf);
	else if (key_word == "return")
		_parse_return(lc_conf);
	else if (key_word == "upload_store")
		_parse_upload_store(lc_conf);
	else if (key_word == "cgi_pass")
		_parse_cgi_pass(lc_conf);
	else
		throw(std::runtime_error("ConfigParser::_parse_location_directive(): Unknown directive name: " + key_word));
}

void	ConfigParser::_parse_allowed_methods(LocationConfig &lc_conf) {
	lc_conf.allowed_methods.clear();
	_match(TOK_WORD);

	while (_peek().type != TOK_SEM) {
		if (_peek().word == "GET")
			lc_conf.allowed_methods.push_back("GET");
		else if (_peek().word == "POST")
			lc_conf.allowed_methods.push_back("POST");
		else if (_peek().word == "DELETE")
			lc_conf.allowed_methods.push_back("DELETE");
		else
			throw(std::runtime_error("ConfigParser::_parse_allowed_methods: Invalid allowed method!"));
		_consume();
	}
	_match(TOK_SEM);
}

void	ConfigParser::_parse_autoindex(LocationConfig &lc_conf) {
	_match(TOK_WORD);
	if (_peek().type != TOK_WORD)
		throw(std::runtime_error("ConfigParser::_parse_autoindex: No valid argument was provided!"));
	if (_peek().word == "on")
		lc_conf.autoindex = true;
	else if (_peek().word == "off")
		lc_conf.autoindex = false;
	else
		throw(std::runtime_error("ConfigParser::_parse_autoindex: Invalid directive argument!" + _peek().word));
	_consume();
	_match(TOK_SEM);
}

void	ConfigParser::_parse_index(LocationConfig &lc_conf) {
	lc_conf.index_files.clear();
	_match(TOK_WORD);
}

WebServerConfig ConfigParser::generate_config(std::string &file_path) {
	Lexer	lexer(file_path);

	_tokens = lexer.tokenizer();
	parse();
	return (_global_config);
}
