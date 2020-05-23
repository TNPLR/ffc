#ifndef PARSER_H_
#define PARSER_H_

#include "ast.h"
#include "lexer.h"

#include <exception>
#include <string>
#include <sstream>

class C_Parser {
public:
	explicit C_Parser(std::string filename);
	Ast parse();
private:
	unsigned int error;
	C_Lexer lexer;
	std::string filename;
	unsigned int row;
	unsigned int column;
	class grammar_exception : public std::exception {
	public:	
		grammar_exception(std::string const& error, std::string const& filename, unsigned row, unsigned column);
		virtual const char *what() const noexcept override;
	private:
		std::string message;
	};
	Ast::Node block_item_list();
	Ast::Node block_item_list_rest(Ast::Node &&node);
	Ast::Node block_item();
	Ast::Node declaration();
	Ast::Node identifier();
	Ast::Node expression_statement();
	Ast::Node expression();
	Ast::Node addictive_expression();
	Ast::Node addictive_expression_rest(Ast::Node &&node);
	Ast::Node multiplicative_expression();
	Ast::Node multiplicative_expression_rest(Ast::Node &&node);
	Ast::Node primary_expression();
	Ast::Node assignment_expression();
};

#endif /* PARSER_H_ */
