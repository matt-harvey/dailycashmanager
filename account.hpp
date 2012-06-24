#ifndef GUARD_account_hpp
#define GUARD_account_hpp

#include "commodity.hpp"
#include "stored_object.hpp"
#include <jewel/decimal.hpp>
#include <string>

// Base class for accounts

class Account: public StoredObject
{
public:
	// typedef for balance type
	// initially let's use cents. We can make it more sophisticated
	// later
	typedef jewel::Decimal BalanceType;
private:
	Commodity m_commodity;
	BalanceType m_balance;
	std::string m_name;
	std::string m_description;
};

// inline member functions



#endif
