#include "vartype.h"
#include <algorithm>
#include <stdexcept>
#include <map>
#include <string>
#include <utility>
#include <functional>

Vartype::Vartype(BasicType t) : _basic_type{t}
{

}

template <typename... E>
Vartype::Vartype(BasicType t, E... e)
{
	_basic_type = t;
	switch (t) {
	case BasicType::FUNC:
		function_construct(e...);
		break;
	case BasicType::STRUCT:
		// structure_construct(e...);
		break;
	case BasicType::PTR:
		pointer_construct(e...);
		break;
	default:
		throw std::invalid_argument{"should not have variable length template"};
	}
}

template <typename T, typename... E>
void Vartype::function_construct(T t, E... e)
{
	_index.push_back(t);
	function_construct(e...);
}

void Vartype::pointer_construct(Vartype const& t)
{
	_point_to.reset(new Vartype{t});
}

std::ostream& operator<<(std::ostream &s, Vartype const& t)
{
	using P = std::pair<std::string, std::function<void()>>;
	auto put = [](std::string s, std::function<void()> f) -> P {
		return std::make_pair(s, f);
	};

	static std::map<Vartype::BasicType, P> name;
	if (name.size() == 0) {
		using Bt = Vartype::BasicType;
		name[Bt::DOUBLE] = put("double", []{});
		name[Bt::INT] = put("int", []{});
		name[Bt::VOID] = put("void", []{});
		name[Bt::PTR] = put("pointer to", []{});
		name[Bt::FUNC] = put("function", [&]{
			s << " return " << t._index.front();
			std::for_each(++t._index.begin(),
				t._index.end(),
				[&](Vartype const& v) {
					s << " parameter "
						<< v;
				});
		});
		name[Bt::STRUCT] = put("structure", []{});
	}

	s << std::get<0>(name[t._basic_type]);
	std::get<1>(name[t._basic_type])();

	return s;
}
