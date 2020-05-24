#include "genasm.h"

#include <algorithm>
#include <sstream>
#include <utility>
#include <map>
#include <tuple>

namespace GenAsm {

class Variable_Position {
public:
	struct StorePos {
		bool reg;
		bool memory;
		bool sync;
		std::size_t regnum;
		std::size_t memnum;
	};

	std::size_t stack_num = 0;

	std::map<Topc::Op, StorePos> mm;
	std::vector<Topc::Op> reg;
	void addmem(Topc::Op);
} vpos;

void Variable_Position::addmem(Topc::Op op)
{
	mm[op] = StorePos{false, true, true, 0, (stack_num += 4)};
}

std::stringstream ss;
static void __check_auto_variable(Topc::Code &c)
{
	Topc::Op op1 = std::get<1>(c);
	Topc::Op op2 = std::get<2>(c);
	Topc::Op op3 = std::get<3>(c);
	if (std::get<0>(op1) == Topc::Position::var) {
		if (vpos.mm.count(op1) == 0) {
			vpos.addmem(op1);
		}
	}
	if (std::get<0>(op2) == Topc::Position::var) {
		if (vpos.mm.count(op2) == 0) {
			vpos.addmem(op2);
		}
	}
	if (std::get<0>(op3) == Topc::Position::var) {
		if (vpos.mm.count(op3) == 0) {
			vpos.addmem(op3);
		}
	}
}

static void __auto_variable(Topc &topc)
{
	for (std::size_t i = 0; i < topc.size(); ++i) {
		__check_auto_variable(topc[i]);
	}
}

static void __generate(Topc &topc)
{
}

std::stringstream& generate(Topc &topc)
{
	ss.flush();
	vpos.reg.resize(9);
	__auto_variable(topc);
	__generate(topc);
	return ss;
}

} // namespace GenAsm
