#include "analyzer.h"
#include "ast.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <functional>
#include <map>
#include <stack>
#include <utility>

namespace Analyzer {

struct Identifier {
public:
	unsigned int num;
	Identifier(unsigned int n) : num{n} {}
	Identifier() : num{0} {}
};

class Scope {
private:
	using Pair = std::pair<std::string, Identifier>;
	std::map<std::string, Identifier> mm;
	std::stack<std::stack<Pair>> ss;
	unsigned int layer;
public:
	Scope() {ss.push(std::stack<Pair>{});};
	void append(std::string name, Identifier i);
	void push();
	void pop();
};

void Scope::append(std::string name, Identifier i)
{
	if (mm.count(name) > 0) {
		ss.top().push(std::make_pair(name, mm[name]));
	}
	mm[name] = i;
}

void Scope::push()
{
	ss.push(std::stack<Pair>{});
}

void Scope::pop()
{
	while (ss.top().size() > 0) {
		Pair p = ss.top().top();
		mm[std::get<0>(p)] = std::get<1>(p);
		ss.top().pop();
	}
	ss.pop();
}

vartype_exception::vartype_exception(std::string const& error,
		unsigned row, unsigned column)
{
	std::stringstream ss;
	ss << row << ':' << column << ": " << error;
	message = ss.str();
}

const char *vartype_exception::what() const noexcept
{
		return message.c_str();
}

static void __declarator_analyze(Ast::Node &node, std::map<std::string, Identifier> &m)
{
	using ExprType = Ast::Node::ExprType;
	using Type = Ast::Node::Type;
	std::for_each(node.begin(), node.end(), [&](auto const &t) {
		m[std::get<std::string>(t->data())] = Identifier{m.size()};
	});
}

static void __variable_analyze(Ast::Node &node, std::map<std::string, Identifier> &m)
{
	using ExprType = Ast::Node::ExprType;
	using Type = Ast::Node::Type;
	if (node.type() == Type::DECLARATION) {
		__declarator_analyze(node, m);
	}
	std::for_each(node.begin(), node.end(),
		[&](auto const &t){
			// for some different scope, map should be passed by value
			__variable_analyze(*t, m);
		});
}

static void __type_analyze_arthematic(Ast::Node &node)
{
}

static void __type_analyze(Ast::Node &node)
{
	using ExprType = Ast::Node::ExprType;
	using Type = Ast::Node::Type;
	std::for_each(node.begin(), node.end(),
		[](auto const &t){__type_analyze(*t);});

	/* set this node type x if the first child is y
		the second is z */
	#define CAL_TYPE(x,y,z) do { \
		if (node[0].exprtype() == y && \
		node[1].exprtype() == z) { \
			node.exprtype(z); \
			return; \
		} } while(0)

	/* set this node type x if one child is y and the other is z */
	#define D_CAL_TYPE(x,y,z) do { \
		if (node[0].exprtype() == y && \
				node[1].exprtype() == z) { \
			node.exprtype(x); \
			return; \
		} else if (node[0].exprtype() == z && \
				node[1].exprtype() == y) { \
			node.exprtype(x); \
			return; \
		}  } while(0)

	/* do the function if one child is t1 and the other is t2 */
	auto if_dtype = [&] (ExprType t1, ExprType t2, std::function<void()> f)->bool { 
		if (node[0].exprtype() == t1 && node[1].exprtype() == t2) {
			f();
			return true;
		}
		if (node[0].exprtype() == t2 && node[1].exprtype() == t1) {
			f();
			return true;
		}
		return false;
	};

	switch (node.type()) {
	case Type::PLUS:
	case Type::MINUS:
	case Type::MUL:
	case Type::DIV:
		D_CAL_TYPE(ExprType::INT, ExprType::INT, ExprType::INT);
		if (if_dtype(ExprType::DOUBLE, ExprType::INT, [&](){
				node.exprtype(ExprType::DOUBLE);
				if (node[0].exprtype() == ExprType::INT) {
					Ast::Node nd{Type::INT_TO_DOUBLE, node[0].row, node[0].column};
					nd.exprtype(ExprType::DOUBLE);
					nd.addson(std::move(node[0]));
					node[0] = std::move(nd);
				} else {
					Ast::Node nd{Type::INT_TO_DOUBLE, node[1].row, node[1].column};
					nd.exprtype(ExprType::DOUBLE);
					nd.addson(std::move(node[1]));
					node[1] = std::move(nd);
				}
			})) {
			return;
		}
		throw vartype_exception{"arthematic type not correct", node.row, node.column};
	case Type::NUMBER:
		return;
	case Type::ID:
		node.exprtype(ExprType::INT);
		return;
	case Type::ASSIGN:
		node.exprtype(ExprType::INT);
		return;
	default:
		break;
	}

	#undef CAL_TYPE
	#undef D_CAL_TYPE
}

void type_analyze(Ast& tree)
{
	__type_analyze(*tree.root);
}

} /* namespace Analyzer */
