#ifndef GUARD_account_hpp
#define GUARD_account_hpp

#include "commodity.hpp"
#include "stored_object.hpp"
#include <string>

// Base class for accounts

class Account: public StoredObject
{
public:
	// typedef for balance type
	// initially let's use cents. We can make it more sophisticated
	// later
	typedef int BalanceType;
private:
	Commodity commodity_;
	BalanceType balance_;
	std::string name_;
	std::string description_;
};

// inline member functions



#endif
