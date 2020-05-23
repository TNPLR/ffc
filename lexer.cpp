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

Lexer::Lexer(std::string filename)
{
	fin.exceptions(std::ifstream::failbit);
	try {
		fin.open(filename);
	} catch (std::ifstream::failure &e) {
		throw e;
	}
	fin.exceptions(std::ifstream::goodbit);
}

void C_Lexer::get_int(int c)
{
	unsigned long long int res = 0;
	do {
		res *= 10;
		res += c - '0';
		c = fin.get();
	} while (isdigit(c));
	fin.unget();
	var = res;
}

void C_Lexer::next()
{
	while (1) {
		int c;
		c = fin.get();
		if (isdigit(c)) {
			get_int(c);
			token = Token::INTEGER;
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