#include "topc.h"
#include <iostream>
#include <ostream>
#include <map>
#include <algorithm>

std::ostream & operator<<(std::ostream &s, Topc::Op op)
{
	static std::map<Topc::Position, std::string> mm;
	if (mm.size() == 0) {
		using P = Topc::Position;
		mm[P::var] = "var";
		mm[P::tmp] = "tmp";
		mm[P::direct] = "direct";
	}
	s << mm[std::get<0>(op)] << ' ' << std::get<1>(op);
	return s;
}
std::ostream & operator<<(std::ostream &s, Topc::Code c)
{
	static std::map<Topc::Instruction, std::string> mm;
	if (mm.size() == 0) {
		using P = Topc::Instruction;
		mm[P::leai] = "leai";
		mm[P::movii] = "movii";
		mm[P::addii] = "addii";
		mm[P::subii] = "subii";
		mm[P::divii] = "divii";
		mm[P::mulii] = "mulii";
		mm[P::loadi] = "loadi";
		mm[P::stoi] = "stoi";
	}
	s << mm[std::get<0>(c)] << ' ' << std::get<1>(c) <<
		", " << std::get<2>(c) << ", " << std::get<3>(c);
	return s;
}

void Topc::print()
{
	std::for_each(vec.begin(), vec.end(), [](Code &c) {
		std::cout << c << '\n';
	});
}
