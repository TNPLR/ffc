#include "lexer.h"
#include "ast.h"
#include "enumprint.h"

#include <ostream>
#include <map>
std::ostream& operator<<(std::ostream &os, C_Lexer::Token t)
{
	static std::map<C_Lexer::Token, std::string> str;
	if (str.size() == 0) {
		#define INSERT_LEXER_ELEMENT(x) str[C_Lexer::Token::x] = #x
		INSERT_LEXER_ELEMENT(PLUS);
		INSERT_LEXER_ELEMENT(MINUS);
		INSERT_LEXER_ELEMENT(MUL);
		INSERT_LEXER_ELEMENT(DIV);
		INSERT_LEXER_ELEMENT(INTEGER);
		INSERT_LEXER_ELEMENT(END);
		INSERT_LEXER_ELEMENT(L_PAR);
		INSERT_LEXER_ELEMENT(R_PAR);
		INSERT_LEXER_ELEMENT(SEMI);
		#undef INSERT_LEXER_ELEMENT
	}
	return os << str[t];
}

std::ostream& operator<<(std::ostream &os, Ast::Node::Type t)
{
	static std::map<Ast::Node::Type, std::string> str;
	if (str.size() == 0) {
		#define INSERT_LEXER_ELEMENT(x) str[Ast::Node::Type::x] = #x
		INSERT_LEXER_ELEMENT(PLUS);
		INSERT_LEXER_ELEMENT(MINUS);
		INSERT_LEXER_ELEMENT(MUL);
		INSERT_LEXER_ELEMENT(DIV);
		INSERT_LEXER_ELEMENT(INTEGER);
		INSERT_LEXER_ELEMENT(EXPR);
		#undef INSERT_LEXER_ELEMENT
	}
	return os << str[t];
}
