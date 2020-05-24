#ifndef VARTYPE_H_
#define VARTYPE_H_
#include <list>
#include <memory>
#include <ostream>

class Vartype {
public:
	enum class BasicType {
		DOUBLE,
		INT,
		PTR,
		FUNC,
		STRUCT,
		VOID,
	};

	Vartype(BasicType t);

	Vartype(Vartype const& t)
	{
		*this = t;
	}

	Vartype & operator=(Vartype const& t)
	{
		_basic_type = t._basic_type;
		if (!(t._point_to == nullptr)) {
			_point_to.reset(new Vartype{*t._point_to});
		}
		_index = t._index;
		return *this;
	}

	friend inline bool operator==(Vartype const& t1, BasicType t2);

	template <typename... E>
	Vartype(BasicType t, E... e);

	BasicType basic_type() const {return _basic_type;}
	void basic_type(BasicType t) {_basic_type = t;}

	friend std::ostream& operator<<(std::ostream &s, Vartype const &t);
private:
	BasicType _basic_type;
	std::unique_ptr<Vartype> _point_to; // pointers

	std::list<Vartype> _index; // function return + parameters
				// structure members

	template <typename T, typename... E>
	void function_construct(T t, E... e);
	inline void function_construct() {}

	void pointer_construct(Vartype const& t);
};

inline bool operator==(Vartype const& t1, Vartype::BasicType t2)
{
	return t1._basic_type == t2;
}
#endif /* VARTYPE_H_ */
