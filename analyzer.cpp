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

static void __declaration_analyze(Ast::Node &node, Scope &m)
{
	using Type = Ast::Node::Type;
	using BasicType = Vartype::BasicType;
}

static void __variable_analyze(Ast::Node &node, Scope &s)
{
	using BasicType = Vartype::BasicType;
	using Type = Ast::Node::Type;
	if (node.type() == Type::DECLARATION) {
		__declaration_analyze(node, s);
	}
	std::for_each(node.begin(), node.end(),
		[&](auto const &t){
			// for some different scope, map should be passed by value
			__variable_analyze(*t, s);
		});
}

static void __type_analyze_arthematic(Ast::Node &node)
{
}

static void __type_analyze(Ast::Node &node)
{
	using BasicType = Vartype::BasicType;
	using Type = Ast::Node::Type;
	std::for_each(node.begin(), node.end(),
		[](auto const &t){__type_analyze(*t);});

	/* set this node type x if the first child is y
		the second is z */
	#define CAL_TYPE(x,y,z) do { \
		if (node[0].vartype() == y && \
		node[1].vartype() == z) { \
			node.vartype(z); \
			return; \
		} } while(0)

	/* set this node type x if one child is y and the other is z */
	#define D_CAL_TYPE(x,y,z) do { \
		if (node[0].vartype() == y && \
				node[1].vartype() == z) { \
			node.vartype(x); \
			return; \
		} else if (node[0].vartype() == z && \
				node[1].vartype() == y) { \
			node.vartype(x); \
			return; \
		}  } while(0)

	/* do the function if one child is t1 and the other is t2 */
	auto if_dtype = [&] (BasicType t1, BasicType t2, std::function<void()> f)->bool { 
		if (node[0].vartype() == t1 && node[1].vartype() == t2) {
			f();
			return true;
		}
		if (node[0].vartype() == t2 && node[1].vartype() == t1) {
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
		D_CAL_TYPE(BasicType::INT, BasicType::INT, BasicType::INT);
		D_CAL_TYPE(BasicType::DOUBLE, BasicType::DOUBLE, BasicType::DOUBLE);
		if (if_dtype(BasicType::DOUBLE, BasicType::INT, [&](){
				node.vartype(BasicType::DOUBLE);
				if (node[0].vartype() == BasicType::INT) {
					Ast::Node nd{Type::INT_TO_DOUBLE, node[0].row, node[0].column};
					nd.vartype(BasicType::DOUBLE);
					nd.addson(std::move(node[0]));
					node[0] = std::move(nd);
				} else {
					Ast::Node nd{Type::INT_TO_DOUBLE, node[1].row, node[1].column};
					nd.vartype(BasicType::DOUBLE);
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
		node.vartype(BasicType::INT);
		return;
	case Type::ASSIGN:
		D_CAL_TYPE(BasicType::INT, BasicType::INT, BasicType::INT);
		D_CAL_TYPE(BasicType::INT, BasicType::INT, BasicType::DOUBLE);

		D_CAL_TYPE(BasicType::DOUBLE, BasicType::DOUBLE, BasicType::DOUBLE);
		D_CAL_TYPE(BasicType::DOUBLE, BasicType::DOUBLE, BasicType::INT);
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
