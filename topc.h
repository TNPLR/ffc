#ifndef TOPC_H_
#define TOPC_H_
#include <vector>
#include <utility>
#include <tuple>
class Topc {
public:
	enum class Position {
		reg,
		mem,
		tmp,
		direct
	};
	enum class Instruction {
		leai,
		movii,
		addii,
		subii,
		divii,
		mulii,
		loadi,
		stoi
	};
	std::size_t tmp_count{0};
	using Op = std::pair<Position, std::size_t>;
	using Code = std::tuple<Instruction, Op, Op, Op>;
	std::vector<Code> vec;
	template <typename T>
	inline void push(T t) {vec.push_back(t);}
	inline void push(Instruction i, Op a, Op b, Op c) {vec.push_back(make_tuple(i, a, b, c));}
	inline Op new_tmp() {return std::make_pair(Position::tmp, ++tmp_count);}
	void print();
};
#endif /* TOPC_H_ */
