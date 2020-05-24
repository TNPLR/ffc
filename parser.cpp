#include "parser.h"

#include "ast.h"
#include "lexer.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using Node = Ast::Node;

C_Parser::grammar_exception::grammar_exception(std::string const& error,
		std::string const& filename, unsigned row, unsigned column)
{
	std::stringstream ss;
	ss << filename << ':' << row << ':' << column << ": " << error;
	message = ss.str();
}

const char *C_Parser::grammar_exception::what() const noexcept
{
		return message.c_str();
}

C_Parser::C_Parser(std::string filename) : error{0}, lexer{filename}, filename{filename}
{
	// Do nothing
}

Ast C_Parser::parse()
{
	lexer.next();
	return Ast{block_item_list()};
}

Ast::Node C_Parser::block_item_list()
{
	Ast::Node res{Node::Type::BLOCK_ITEM, lexer.row(), lexer.column()};
	block_item(res);
	return block_item_list_rest(std::move(res));
}

Ast::Node C_Parser::block_item_list_rest(Ast::Node &&node)
{
	using Token = C_Lexer::Token;
	switch (lexer.token) {
	case Token::R_PAR:
	case Token::END:
		return node;
	default:
		break;
	}
	block_item(node);
	return block_item_list_rest(std::forward<Ast::Node>(node));
}

void C_Parser::block_item(Ast::Node &list)
{
	Ast::Node res{lexer.row(), lexer.column()};
	switch (lexer.token) {
	case C_Lexer::Token::INT:
	case C_Lexer::Token::DOUBLE:
		res = declaration();
		std::for_each(res.begin(), res.end(), [&](auto const& ptr) {list.addson(*ptr);});
		return;
	default:
		list.addson(expression_statement());
		return;
	}
}

Ast::Node C_Parser::declarator(Ast::Node &node)
{
	Ast::Node res{Node::Type::DECLARATION, lexer.row(), lexer.column()};
	Ast::Node id{identifier()};
	res.exprtype(node.exprtype());
	res.data(id.data());
	return res;
}

Ast::Node C_Parser::declaration()
{
	Ast::Node res{Node::Type::DECLARATION, lexer.row(), lexer.column()};
	switch (lexer.token) {
	case C_Lexer::Token::INT:
		lexer.pop();
		res.exprtype(Ast::Node::ExprType::INT);
		res.addson(declarator(res));
		break;
	case C_Lexer::Token::DOUBLE:
		lexer.pop();
		res.exprtype(Ast::Node::ExprType::DOUBLE);
		res.addson(declarator(res));
		break;
	default:
		throw grammar_exception{"cannot find a declaration specifier", filename, row, column};
	}
	lexer.match_and_pop(C_Lexer::Token::SEMI);
	return res;
}

Ast::Node C_Parser::expression_statement()
{
	Ast::Node res{Node::Type::EXPR, lexer.row(), lexer.column()};
	try {
		res.addson(expression());
		lexer.match_and_pop(C_Lexer::Token::SEMI);
	} catch (grammar_exception &e) {
		std::cerr << e.what() << '\n';
		++error;
		while (true) {
			switch (lexer.token) {
			case C_Lexer::Token::SEMI:
			case C_Lexer::Token::END:
				lexer.pop();
				return res;
			default:
				break;
			}
			lexer.pop();
		}
	} catch (Lexer::lexical_exception &e) {
		std::cerr << e.what() << '\n';
		++error;
		while (true) {
			switch (lexer.token) {
			case C_Lexer::Token::SEMI:
			case C_Lexer::Token::END:
				lexer.pop();
				return res;
			default:
				break;
			}
			lexer.pop();
		}
	}
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
		Ast::Node res{Node::Type::ASSIGN, lexer.row(), lexer.column()};
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
	Node res{lexer.row(), lexer.column()};
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
	Node res{lexer.row(), lexer.column()};
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

Ast::Node C_Parser::identifier()
{
	Node res{Node::Type::ID, lexer.row(), lexer.column()};
	res.data(std::get<std::string>(lexer.var));
	lexer.match_and_pop(C_Lexer::Token::ID);
	return res;
}

Ast::Node C_Parser::primary_expression()
{
	using Token = C_Lexer::Token;
	Node res{lexer.row(), lexer.column()};
	switch (lexer.token) {
	case Token::INTEGER:
		res.type(Node::Type::NUMBER);
		res.exprtype(Node::ExprType::INT);
		res.data(std::get<unsigned long long int>(lexer.var));
		lexer.pop();
		return res;
	case Token::FLOATING:
		res.type(Node::Type::NUMBER);
		res.exprtype(Node::ExprType::DOUBLE);
		res.data(std::get<double>(lexer.var));
		lexer.pop();
		return res;
	case Token::L_PAR:
		lexer.pop();
		res = expression();
		lexer.match_and_pop(C_Lexer::Token::R_PAR);
		return res;
	case Token::ID:
		return identifier();
	default:
		throw grammar_exception("expected a primary expression.", filename, lexer.row(), lexer.column());
	}
}
