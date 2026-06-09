#include "../config_file_parser/Lexer.hpp"
#include <iostream>

int main(int ac, char **av) {
	std::vector<Token>	tokens;
	if (ac != 2) {
		std::cout << "Invalid number of args!\n";
		return (0);
	}
	Lexer config_lexer(av[1]);
	tokens = config_lexer.tokenizer();
	for (size_t i = 0; i < tokens.size(); i += 1) {
		std::cout << tokens[i].type << ": " << tokens[i].word << std::endl;
	}


	return (0);
}
