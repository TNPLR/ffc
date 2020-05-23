#ifndef PARSER_H_
#define PARSER_H_

#include "ast.h"
#include "lexer.h"

#include <string>

class C_Parser {
public:
	explicit C_Parser(std::string filename);
	Ast parse();
private:
	C_Lexer lexer;
	Ast::Node block_item_list();
	Ast::Node block_item_list_rest(Ast::Node &&node);
	Ast::Node block_item();
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
