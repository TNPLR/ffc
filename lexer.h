#ifndef LEXER_H_
#define LEXER_H_
#include <string>
#include <fstream>
#include <variant>
#include <ostream>

class Lexer {
public:
	explicit Lexer(std::string filename);
	virtual void next() = 0;
	unsigned int row;
	unsigned int column;
protected:
	int lget();
	void lunget();
	std::ifstream fin;
};

class C_Lexer : public Lexer {
public:
	enum class Token {
		PLUS,
		MINUS,
		MUL,
		DIV,
		INTEGER,
		END,
		L_PAR,
		R_PAR,
		SEMI,
		INT,
		ID,
		ASSIGN
	} token;
	std::variant<unsigned long long int, double, std::string> var;
	explicit C_Lexer(std::string filename);
	void next() override;
	void match_and_pop(Token t);
	inline void pop() {next();};
private:
	void get_int(int c);
	void get_identifier(int c);
};
std::ostream& operator<<(std::ostream &os, C_Lexer::Token);
#endif /* LEXER_H_ */
