#include "lexer.h"
#include "enumprint.h"

#include <cstdlib>
#include <cctype>

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <map>

C_Lexer::C_Lexer(std::string filename) : Lexer{filename}
{
	// Do nothing
}

Lexer::Lexer(std::string filename) : row{1}, column{1}
{
	fin.exceptions(std::ifstream::failbit);
	try {
		fin.open(filename);
	} catch (std::ifstream::failure &e) {
		throw e;
	}
	fin.exceptions(std::ifstream::goodbit);
}

int Lexer::lget()
{
	int c = fin.get();
	if (!fin.eof()) {
		++column;
	}
	if (c == '\n') {
		++row;
		column = 1;
	}
	return c;
}

void Lexer::lunget()
{
	fin.unget();
	if (column == 1) {
		if (row == 1) {
			return;
		} else if (row == 2) {
			std::streampos pos = fin.tellg();
			unsigned int n_column = 0;
			fin.seekg(0, std::ios_base::beg);
			while (fin.get() != '\n') {
				++n_column;
			}
			fin.seekg(pos);
			row = 1;
			column = n_column;
		} else {
			std::streampos pos = fin.tellg();
			unsigned int n_column = 1;
			while (fin.unget(), fin.get() != '\n') {
				++n_column;
				fin.unget();
			}
			fin.seekg(pos);
			row = 1;
			column = n_column;
		}
	}
}

void C_Lexer::get_int(int c)
{
	unsigned long long int res = 0;
	do {
		res *= 10;
		res += c - '0';
		c = lget();
	} while (isdigit(c));
	lunget();
	var = res;
}

void C_Lexer::get_identifier(int c)
{
	std::string str{""};
	do {
		str += c;
		c = lget();
	} while (isalpha(c) || isdigit(c) || c == '_');
	lunget();

	static std::map<std::string, C_Lexer::Token> keyword;
	if (keyword.size() == 0) {
		keyword["int"] = C_Lexer::Token::INT;
	}

	if (keyword.count(str) != 0) {
		token = keyword[str];
		return;
	}

	token = C_Lexer::Token::ID;
	var = str;
}

void C_Lexer::next()
{
	while (1) {
		int c;
		c = lget();
		if (isdigit(c)) {
			get_int(c);
			token = Token::INTEGER;
			return;
		}
		if (isalpha(c) || c == '_') {
			get_identifier(c);
			return;
		}
		switch (c) {
		case -1:
			token = Token::END;
			return;
		case '+':
			token = Token::PLUS;
			return;
		case '-':
			token = Token::MINUS;
			return;
		case '*':
			token = Token::MUL;
			return;
		case '/':
			token = Token::DIV;
			return;
		case '(':
			token = Token::L_PAR;
			return;
		case ')':
			token = Token::R_PAR;
			return;
		case '=':
			token = Token::ASSIGN;
			return;
		case ';':
			token = Token::SEMI;
			return;
		default:
			break;
		}
	}
}

void C_Lexer::match_and_pop(Token t)
{
	if (token != t) {
		std::cerr << "Expected Token " << t << ", but found " << token << ".\n";
		exit(1);
	}
	next();
}
