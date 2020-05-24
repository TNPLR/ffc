#include "ast.h"
#include "parser.h"
#include "analyzer.h"
#include "gentopc.h"
#include "topc.h"

#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "No filename entered\n";
		return 1;
	}

	C_Parser parser{argv[1]};
	Ast tree{parser.parse()};
	Analyzer::set_filename(argv[1]);
	Analyzer::type_analyze(tree);
	tree.sexp_print();

	Topc topc{GenTopc::generate(tree)};
	topc.print();

	return 0;
}
