#ifndef ANALYZER_H_
#define ANALYZER_H_
#include "ast.h"
#include <string>
namespace Analyzer {

void set_filename(std::string const&);
std::size_t type_analyze(Ast& tree);

class vartype_exception : public std::exception {
public:	
	vartype_exception(std::string const& error, unsigned row, unsigned column);
	virtual const char *what() const noexcept override;
private:
	std::string message;
};

class var_redefined_exception : public std::exception {
public:	
	var_redefined_exception(std::string const& error);
	virtual const char *what() const noexcept override;
private:
	std::string message;
};

class var_undefined_exception : public std::exception {
public:	
	var_undefined_exception(std::string const& error);
	virtual const char *what() const noexcept override;
private:
	std::string message;
};

} // namespace Analyzer
#endif /* ANALYZER_H_ */
