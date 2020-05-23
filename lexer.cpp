#include "lexer.h"
#include "enumprint.h"

#include <cstdlib>
#include <cctype>

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <map>
#include <sstream>

C_Lexer::lexical_exception::lexical_exception(std::string const& error,
		std::string const& filename, unsigned row, unsigned column)
{
	std::stringstream ss;
	ss << filename << ':' << row << ':' << column << ": " << error;
	message = ss.str();
}

const char *C_Lexer::lexical_exception::what() const noexcept
{
	return message.c_str();
}

C_Lexer::C_Lexer(std::string filename) : Lexer{filename}
{
	// Do nothing
}

Lexer::Lexer(std::string filename) : _row{1}, _column{0}, filename{filename}
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
		++_column;
	}
	if (c == '\n') {
		++_row;
		_column = 0;
	}
	return c;
}

void Lexer::lunget()
{
	fin.unget();
	if (_column == 1) {
		if (_row == 1) {
			--_column;
			return;
		} else if (_row == 2) {
			std::streampos pos = fin.tellg();
			unsigned int n__column = 0;
			fin.seekg(0, std::ios_base::beg);
			while (fin.get() != '\n') {
				++n__column;
			}
			fin.seekg(pos);
			_row = 1;
			_column = n__column;
		} else {
			std::streampos pos = fin.tellg();
			unsigned int n__column = 1;
			while (fin.unget(), fin.get() != '\n') {
				++n__column;
				fin.unget();
			}
			fin.seekg(pos);
			_row = 1;
			_column = n__column;
		}
	} else if (_column == 0) {
		return;
	} else {
		--_column;
		return;
	}
}

void C_Lexer::get_double(unsigned long long int l)
{
	int c = lget();
	double d{static_cast<double>(l)};
	double div = 0.1;
	while (isdigit(c)) {
		d += static_cast<double>(c - '0') * div;
		div /= 10.0;
		c = lget();
	}
	lunget();
	var = d;
	token = Token::DOUBLE;
}

void C_Lexer::get_int(int c)
{
	unsigned long long int res = 0;
	do {
		res *= 10;
		res += c - '0';
		c = lget();
	} while (isdigit(c));

	if (c == '.') {
		get_double(res);
		return;
	}

	lunget();
	var = res;
	token = Token::INTEGER;
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
		std::stringstream ss;
		ss << "Expected Token " << t << ", but found " << token << ".";
		throw lexical_exception(ss.str(), filename, row(), column());
	}
	next();
}
