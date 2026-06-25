#include "./Lexer.hpp"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(void) : _config_file_path("webserv.conf") {
	open_config_file(_config_file_path);
}

Lexer::~Lexer(void) {
	_config_file.close();
}
Lexer::Lexer(const std::string &file_path) : _config_file_path(file_path) {
	open_config_file(_config_file_path);
}

void Lexer::open_config_file(const std::string &file_path) {
	_config_file.open(file_path.c_str());

	if (_config_file.is_open() == false)
		throw(std::runtime_error("Failed to open config file!"));
}

bool Lexer::_is_symbol(char c) {
	if (c == ';')
		return (true);
	if (c == '{')
		return (true);
	if (c == '}')
		return (true);
	return (false);
}

std::string::size_type Lexer::extract_token(std::string &line, size_t peek,
					    Token &token) {
	token.word.clear();
	for (std::string::size_type i = peek; i < line.size(); i += 1) {
		if (_is_symbol(line[i])) {
			if (line[i] == ';') {
				token.word = ";";
				token.type = TOK_SEM;
			} else if (line[i] == '{') {
				token.word = "{";
				token.type = TOK_OPEN_BR;
			} else if (line[i] == '}') {
				token.word = "}";
				token.type = TOK_CLOSE_BR;
			}
			return (i + 1);
		} else if (isspace(line[i]) == false) {
			while (i < line.size() && isspace(line[i]) == false &&
			       !_is_symbol(line[i])) {
				token.word.push_back(line[i]);
				i += 1;
			}
			token.type = TOK_WORD;
			return (i);
		}
	}
	return (peek);
}

std::vector<Token> Lexer::tokenizer(void) {
	std::string line;
	std::vector<Token> tokens;
	Token temp;
	std::string::size_type peek;

	while (std::getline(_config_file, line)) {
		/* Remove all the part that contains a comment from a line */
		std::string::size_type comment_start = line.find('#');
		if (comment_start == 0)
			continue;
		if (comment_start != std::string::npos)
			line = line.substr(0, comment_start);
		peek = 0;
		while (peek < line.size()) {
			std::string::size_type next_peek;
			next_peek = extract_token(line, peek, temp);
			if (next_peek == peek)
				break;
			peek = next_peek;
			tokens.push_back(temp);
		}
	}
	temp.word = "";
	temp.type = TOK_EOF;
	tokens.push_back(temp);
	return (tokens);
}
