#include "parser.h"

#include "ast.h"
#include "lexer.h"

#include <iostream>
#include <fstream>

using Node = Ast::Node;

C_Parser::C_Parser(std::string filename) : lexer{filename}
{
	// Do nothing
}

Ast C_Parser::parse()
{
	lexer.next();
	return Ast{expression_statement()};
}

Ast::Node C_Parser::expression_statement()
{
	Ast::Node res{Node::Type::EXPR};
	res.addson(expression());
	lexer.match_and_pop(C_Lexer::Token::SEMI);
	return res;
}

Ast::Node C_Parser::expression()
{
	return assignment_expression();
}

Ast::Node C_Parser::assignment_expression()
{
	using Token = C_Lexer::Token;
	Ast::Node left = addictive_expression();
	if (lexer.token == Token::ASSIGN) {
		lexer.pop();
		Ast::Node res{Node::Type::ASSIGN};
		res.addson(std::move(left));
		res.addson(assignment_expression());
		return res;
	}
	return left;
}

Node C_Parser::addictive_expression()
{
	return addictive_expression_rest(multiplicative_expression());
}

Ast::Node C_Parser::addictive_expression_rest(Ast::Node &&node)
{
	using Token = C_Lexer::Token;
	Node res{};
	switch (lexer.token) {
	case Token::PLUS:
		res.type(Node::Type::PLUS);
		res.addson(node);
		lexer.pop();
		res.addson(multiplicative_expression());
		return addictive_expression_rest(std::move(res));
	case Token::MINUS:
		res.type(Node::Type::MINUS);
		res.addson(node);
		lexer.pop();
		res.addson(multiplicative_expression());
		return addictive_expression_rest(std::move(res));
	default:
		return node;
	}
}

Ast::Node C_Parser::multiplicative_expression()
{
	return multiplicative_expression_rest(primary_expression());
}

Ast::Node C_Parser::multiplicative_expression_rest(Ast::Node &&node)
{
	using Token = C_Lexer::Token;
	Node res{};
	switch (lexer.token) {
	case Token::MUL:
		res.type(Node::Type::MUL);
		res.addson(node);
		lexer.pop();
		res.addson(primary_expression());
		return multiplicative_expression_rest(std::move(res));
	case Token::DIV:
		res.type(Node::Type::DIV);
		res.addson(node);
		lexer.pop();
		res.addson(primary_expression());
		return multiplicative_expression_rest(std::move(res));
	default:
		return node;
	}
}

Ast::Node C_Parser::primary_expression()
{
	using Token = C_Lexer::Token;
	Node res{};
	switch (lexer.token) {
	case Token::INTEGER:
		res.type(Node::Type::INTEGER);
		res.data(std::get<unsigned long long int>(lexer.var));
		lexer.pop();
		return res;
	case Token::L_PAR:
		lexer.pop();
		res = expression();
		lexer.match_and_pop(C_Lexer::Token::R_PAR);
		return res;
	case Token::ID:
		res.type(Node::Type::ID);
		res.data(std::get<std::string>(lexer.var));
		lexer.pop();
		return res;
	default:
		lexer.match_and_pop(C_Lexer::Token::INTEGER);
		throw 1;
	}
}
