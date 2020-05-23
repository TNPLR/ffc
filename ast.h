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
	using DataType = std::variant<unsigned long long int, double, std::string>;
	enum class Type {
		EXPR,
		PLUS,
		MINUS,
		MUL,
		DIV,
		NUMBER,
		ID,
		ASSIGN,
		BLOCK_ITEM,
		DECLARATION,
		INT_TO_DOUBLE,
	};

	enum class ExprType {
		UNDEF,
		INT,
		DOUBLE
	};

	explicit Node() = delete;
	explicit Node(unsigned int row, unsigned int column) : row{row}, column{column} {}
	explicit Node(Type t, unsigned int row, unsigned int column);
	Node(Node const&);
	Node(Node &&);

	Node &operator=(Node &&);

	~Node();

	void addson(Node& node);
	void addson(Node&& node);

	inline void exprtype(ExprType t); // Set
	inline ExprType exprtype(); // Set

	inline void type(Type t); // Set
	inline Type type(); // Get

	template <typename T>
	inline void data(T t); // Set
	inline DataType const& data(); // Get

	Node& operator[](std::size_t s)
	{
		return *son_node[s];
	}
	unsigned int row;
	unsigned int column;

	void sexp_print();
private:
	void _sexp_print(int level);
	DataType _data;
	enum Type _type;
	ExprType _exprtype{ExprType::UNDEF};
	std::vector<std::shared_ptr<Node>> son_node;
public:
	auto begin() -> decltype(son_node.begin())
	{
		return son_node.begin();
	}

	auto end() -> decltype(son_node.end())
	{
		return son_node.end();
	}

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

inline void Ast::Node::exprtype(ExprType t)
{
	_exprtype = t;
}

inline Ast::Node::ExprType Ast::Node::exprtype()
{
	return _exprtype;
}
#endif /* AST_H_ */
