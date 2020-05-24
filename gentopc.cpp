#include "gentopc.h"
#include <utility>
#include <algorithm>

namespace GenTopc {

static Topc::Op __gen_lvalue(Ast::Node &node, Topc &topc)
{
	using Type = Ast::Node::Type;
	using I = Topc::Instruction;
	Topc::Op nop;
	switch (node.type()) {
	case Type::ID:
		return std::make_pair(Topc::Position::var, std::get<Identifier>(node.data()).num);
	default:
		throw 0; // not a lvalue
	}
}

static Topc::Op __generate(Ast::Node &node, Topc &topc)
{
	using Type = Ast::Node::Type;
	using I = Topc::Instruction;
	using Op = Topc::Op;
	Topc::Op result;
	Topc::Op lop;
	Topc::Op rop;
	switch (node.type()) {
	case Type::ASSIGN:
		lop = __gen_lvalue(node[0], topc);
		rop = __generate(node[1], topc);
		topc.push(I::stoi, lop, rop, Op{});
		return result;
	case Type::PLUS:
		result = topc.new_tmp();
		lop = __generate(node[0], topc);
		rop = __generate(node[1], topc);
		topc.push(I::addii, result, lop, rop);
		return result;
	case Type::MINUS:
		result = topc.new_tmp();
		lop = __generate(node[0], topc);
		rop = __generate(node[1], topc);
		topc.push(I::subii, result, lop, rop);
		return result;
	case Type::MUL:
		result = topc.new_tmp();
		lop = __generate(node[0], topc);
		rop = __generate(node[1], topc);
		topc.push(I::mulii, result, lop, rop);
		return result;
	case Type::DIV:
		result = topc.new_tmp();
		lop = __generate(node[0], topc);
		rop = __generate(node[1], topc);
		topc.push(I::divii, result, lop, rop);
		return result;
	case Type::NUMBER:
		result = topc.new_tmp();
		topc.push(I::loadi, result, Op{Topc::Position::direct, std::get<unsigned long long int>(node.data())}, Op{});
		return result;
	case Type::ID:
		result = topc.new_tmp();
		topc.push(I::loadi, result, std::make_pair(Topc::Position::var, std::get<Identifier>(node.data()).num), Op{});
		return result;
	default:
		std::for_each(node.begin(), node.end(), [&](std::shared_ptr<Ast::Node> &n){ __generate(*n, topc);});
		return Op{};
	}
}

Topc generate(Ast &tree)
{
	Topc topc{};
	__generate(*tree.root, topc);
	return topc;
}

} // namespace GenTopc
