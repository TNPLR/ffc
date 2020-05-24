#include "analyzer.h"
#include "ast.h"
#include "vartype.h"
#include "identifier.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <functional>
#include <map>
#include <stack>
#include <utility>

namespace Analyzer {

std::string filename;
std::size_t error_count;

void set_filename(std::string const& s)
{
	filename = s;
}

class Scope {
private:
	std::size_t total_count{1};
	using Pair = std::pair<std::string, Identifier>;
	std::map<std::string, Identifier> mm;
	std::stack<std::list<Pair>> ss;
	unsigned int layer;
public:
	Scope() {ss.push(std::list<Pair>{});};
	void append(std::string const& name, Vartype i);
	void push();
	void pop();

	Vartype get_type(std::string name);
	Identifier get_id(std::string name);
};


Vartype Scope::get_type(std::string name)
{
	if (mm.count(name) == 0) {
		throw var_undefined_exception("variable \"" + name + "\" not found");
	}
	return (mm[name].type);
}

Identifier Scope::get_id(std::string name)
{
	if (mm.count(name) == 0) {
		throw var_undefined_exception("variable \"" + name + "\" not found");
	}
	return mm[name];
}

void Scope::append(std::string const& name, Vartype i)
{
	Identifier id{++total_count, i, name};
	if (mm.count(name) > 0) {
		for (auto it = ss.top().begin(); it != ss.top().end(); ++it) {
			if (name == std::get<0>(*it)) {
				throw var_redefined_exception("variable \"" + name + "\" redefined");
			}
		}
		ss.top().push_front(std::make_pair(name, mm[name]));
	} else {
		ss.top().push_front(std::make_pair(name, Identifier{0, Vartype::BasicType::VOID, ""}));
	}
	mm[name] = id;
}

void Scope::push()
{
	ss.push(std::list<Pair>{});
}

void Scope::pop()
{
	while (ss.top().size() > 0) {
		Pair p = ss.top().front();
		if (std::get<1>(p).num == 0) {
			mm.erase(std::get<0>(p));
		} else {
			mm[std::get<0>(p)] = std::get<1>(p);
		}
		ss.top().pop_front();
	}
	ss.pop();
}

vartype_exception::vartype_exception(std::string const& error,
		unsigned row, unsigned column)
{
	std::stringstream ss;
	ss << filename << ':' << row << ':' << column << ": " << error;
	message = ss.str();
}

const char *vartype_exception::what() const noexcept
{
	return message.c_str();
}

var_redefined_exception::var_redefined_exception(std::string const& error)
{
	message = error;
}

const char *var_redefined_exception::what() const noexcept
{
	return message.c_str();
}

var_undefined_exception::var_undefined_exception(std::string const& error)
{
	message = error;
}

const char *var_undefined_exception::what() const noexcept
{
	return message.c_str();
}

static void __declaration_analyze(Ast::Node &node, Scope &m)
{
	using Type = Ast::Node::Type;
	using BasicType = Vartype::BasicType;
	try {
		m.append(std::get<std::string>(node.data()), node.vartype());
	} catch (var_redefined_exception &e) {
		std::cerr << filename << ':' << node.row <<
			':' << node.column << ':' <<
			e.what() << '\n';
		++error_count;
	}
}

static void __vartype_filling(Ast::Node &node, Scope &s)
{
	Identifier id{s.get_id(std::get<std::string>(node.data()))};
	node.vartype(id.type);
	node.data(id);
}

static void __variable_analyze(Ast::Node &node, Scope &s)
{
	using BasicType = Vartype::BasicType;
	using Type = Ast::Node::Type;
	if (node.type() == Type::DECLARATION) {
		__declaration_analyze(node, s);
	} else if (node.type()  == Type::ID) {
		__vartype_filling(node, s);
	}
	std::for_each(node.begin(), node.end(),
		[&](auto const &t){
			// for some different scope, map should be passed by value
			__variable_analyze(*t, s);
		});
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

	/* do the function if child 0 is t1 and child 1 is t2 */
	auto if_type = [&] (BasicType t1, BasicType t2, std::function<void()> f)->bool { 
		if (node[0].vartype() == t1 && node[1].vartype() == t2) {
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
		return;
	case Type::ASSIGN:
		D_CAL_TYPE(BasicType::INT, BasicType::INT, BasicType::INT);
		D_CAL_TYPE(BasicType::INT, BasicType::INT, BasicType::DOUBLE);

		D_CAL_TYPE(BasicType::DOUBLE, BasicType::DOUBLE, BasicType::DOUBLE);
		D_CAL_TYPE(BasicType::DOUBLE, BasicType::DOUBLE, BasicType::INT);

		if (if_type(BasicType::DOUBLE, BasicType::INT, [&](){
				node.vartype(BasicType::DOUBLE);
				Ast::Node nd{Type::INT_TO_DOUBLE, node[0].row, node[0].column};
				nd.vartype(BasicType::DOUBLE);
				nd.addson(std::move(node[0]));
				node[0] = std::move(nd);
			})) {
			return;
		}

		if (if_type(BasicType::INT, BasicType::DOUBLE, [&](){
				node.vartype(BasicType::INT);
				Ast::Node nd{Type::DOUBLE_TO_INT, node[0].row, node[0].column};
				nd.vartype(BasicType::INT);
				nd.addson(std::move(node[0]));
				node[0] = std::move(nd);
			})) {
			return;
		}
		throw vartype_exception{"assignment type not correct", node.row, node.column};
	default:
		break;
	}

	#undef CAL_TYPE
	#undef D_CAL_TYPE
}

std::size_t type_analyze(Ast& tree)
{
	Scope s{};
	__variable_analyze(*tree.root, s);
	__type_analyze(*tree.root);
	return error_count;
}

} /* namespace Analyzer */
