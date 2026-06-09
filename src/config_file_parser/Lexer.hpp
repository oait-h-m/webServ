#ifndef LEXER_HPP
#define LEXER_HPP

#include <fstream>
#include <string>
#include <vector>

enum TokenType { TOK_WORD, TOK_OPEN_BR, TOK_CLOSE_BR, TOK_SEM, TOK_EOF };

struct Token {
	std::string word;
	TokenType type;
};

class Lexer {
	private:
		Lexer(const Lexer &other);
		Lexer &operator=(const Lexer &other);
		std::ifstream _config_file;
		std::string _config_file_path;
		std::vector<Token> _tokens;
		void _strip_comments(std::string &buffer);
		bool _is_symbol(char c);
		TokenType _identify_token(const std::string &word);
		size_t	extract_token(std::string &line, size_t peek, Token &token);

	public:
		void open_config_file(const std::string &file_path);
		Lexer(void);
		Lexer(const std::string &file_path);
		std::vector<Token> tokenizer(void);
		~Lexer();
};

#endif /* LEXER_HPP */
