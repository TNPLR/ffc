#ifndef AST_H_
#define AST_H_

#include <vector>
#include <memory>
#include <variant>

class Ast {
public:
	class Node;
	std::shared_ptr<Node> root;
	explicit Ast(Node &&ptr);
	void sexp_print();
};

class Ast::Node {
public:
	using DataType = std::variant<unsigned long long int, std::string>;
	enum class Type {
		EXPR,
		PLUS,
		MINUS,
		MUL,
		DIV,
		INTEGER,
		ID,
		ASSIGN,
		BLOCK_ITEM,
		DECLARATION,
	};

	explicit Node() = default;
	explicit Node(Type t);
	Node(Node &);
	Node(Node &&);

	Node &operator=(Node &&);

	~Node();

	void addson(Node& node);
	void addson(Node&& node);

	inline void type(Type t); // Set
	inline Type type(); // Get

	template <typename T>
	inline void data(T t); // Set
	inline DataType const& data(); // Get

	void sexp_print();
private:
	void _sexp_print(int level);
	DataType _data;
	enum Type _type;
	std::vector<std::shared_ptr<Node>> son_node;
};

template <typename T>
inline void Ast::Node::data(T t)
{
	_data = t;
}

inline Ast::Node::DataType const& Ast::Node::data()
{
	return _data;
}

inline void Ast::Node::type(Type t)
{
	_type = t;
}

inline Ast::Node::Type Ast::Node::type()
{
	return _type;
}

#endif /* AST_H_ */
