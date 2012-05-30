#ifndef GUARD_commodity_hpp
#define GUARD_commodity_hpp

#include <string>

class Commodity
{
public:
	std::string str_rep() const;
private:
	std::string str_rep_;
};

// inline member functions

inline std::string Commodity::str_rep() const
{
	return str_rep_;
}


#endif
