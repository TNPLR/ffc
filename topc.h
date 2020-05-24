#ifndef TOPC_H_
#define TOPC_H_
#include <tuple>
class Topc {
public:
	using Code = std::tuple<Instruction, std::size_t, std::size_t, std::size_t>;
	enum class Instruction {
		movii,
		addii,
		subii,
		divii,
		mulii,
		loadi,
	};
	std::vector<Code> vec;
};
#endif /* TOPC_H_ */
