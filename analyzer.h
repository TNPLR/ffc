#ifndef ANALYZER_H_
#define ANALYZER_H_
#include "ast.h"
namespace Analyzer {
class vartype_exception : public std::exception {
public:	
	vartype_exception(std::string const& error, unsigned row, unsigned column);
	virtual const char *what() const noexcept override;
private:
	std::string message;
};
void type_analyze(Ast& tree);
}
#endif /* ANALYZER_H_ */
