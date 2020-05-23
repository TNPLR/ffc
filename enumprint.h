// This file must be included after other headers are included
#ifndef ENUMPRINT_H_
#define ENUMPRINT_H_

#include <ostream>

#ifdef LEXER_H_
std::ostream& operator<<(std::ostream &os, C_Lexer::Token t);
#endif /* LEXER_H_ */

#ifdef AST_H_
std::ostream& operator<<(std::ostream &os, Ast::Node::Type t);
std::ostream& operator<<(std::ostream &os, Ast::Node::ExprType t);
#endif /* AST_H_ */

#endif /* ENUMPRINT_H_ */
