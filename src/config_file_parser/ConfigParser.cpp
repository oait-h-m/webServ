#include "./ConfigParser.hpp"

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
