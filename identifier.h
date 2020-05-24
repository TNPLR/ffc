#ifndef IDENTIFIER_H_
#define IDENTIFIER_H_
#include "vartype.h"
#include <string>

// num 0 is reserved for invalid or null identifier
struct Identifier {
public:
	std::size_t num;
	Vartype type;
	std::string name;
	Identifier(std::size_t n, Vartype t, std::string s) : num{n}, type{t}, name{s} {}
	Identifier() : num{1}, type{Vartype::BasicType::VOID}, name{""} {}
};

#endif /* IDENTIFIER_H_ */
