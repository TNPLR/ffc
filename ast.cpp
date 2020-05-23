#include "ast.h"

#include "enumprint.h"

#include <memory>
#include <algorithm>
#include <iostream>
#include <utility>

Ast::Ast(Node &&node) : root{new Node{std::forward<Node>(node)}}
{
	// Do nothing
}

Ast::Node::Node(Type t) : _type{t}
{
	// Do nothing
}

Ast::Node::Node(Node &node) : _data{node._data}, _type{node._type}
{
	son_node.resize(node.son_node.size());
	for (std::size_t i = 0; i < node.son_node.size(); ++i) {
		son_node[i] = std::shared_ptr<Node>{new Node{*node.son_node[i]}};
	}
}

Ast::Node::Node(Node &&node) : _data{std::move(node._data)}, _type{node._type}, son_node{std::move(node.son_node)}
{
	// Do nothing
}

Ast::Node &Ast::Node::operator=(Node &&node)
{
	if (this != &node) {
		_data = std::move(node._data);
		_type = std::move(node._type);
		son_node = std::move(node.son_node);
	}
	return *this;
}

void Ast::Node::addson(Node& node)
{
	son_node.push_back(std::shared_ptr<Node>{new Node{node}});
}

void Ast::Node::addson(Node&& node)
{
	son_node.push_back(std::shared_ptr<Node>{new Node{std::forward<Node>(node)}});
}

Ast::Node::~Node()
{
	// Do nothing
}

void Ast::sexp_print()
{
	root->sexp_print();
	std::cout << std::endl;
}

void Ast::Node::sexp_print()
{
	_sexp_print(0);
}

void Ast::Node::_sexp_print(int level)
{
	for (int i = 0; i < level; ++i) {
		std::cout << '\t';
	}
	std::cout << "(" << _type;
	if (_type == Type::INTEGER) {
		std::cout << ' ' << std::get<unsigned long long int>(_data);
	} else if (_type == Type::ID) {
		std::cout << ' ' << std::get<std::string>(_data);
	}
	std::for_each(son_node.begin(), son_node.end(),
		[level](std::shared_ptr<Node> const& nd) {
			std::cout << '\n';
			nd->_sexp_print(level + 1);
		});
	std::cout << ")";
}
