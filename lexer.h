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
	unsigned int row() const {return _row;};
	unsigned int column() const {return _column;};

	class lexical_exception : public std::exception {
	public:	
		lexical_exception(std::string const& error, std::string const& filename, unsigned row, unsigned column);
		virtual const char *what() const noexcept override;
	private:
		std::string message;
	};

protected:
	int lget();
	void lunget();

	unsigned int _row;
	unsigned int _column;
	std::string filename;
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
		ASSIGN,
		FLOATING,
		DOUBLE,
	} token;
	std::variant<unsigned long long int, double, std::string> var;
	explicit C_Lexer(std::string filename);
	void next() override;
	void match_and_pop(Token t);
	inline void pop() {next();};
private:
	void get_int(int c);
	void get_double(unsigned long long int c);
	void get_identifier(int c);
};
std::ostream& operator<<(std::ostream &os, C_Lexer::Token);
#endif /* LEXER_H_ */
