#ifndef GUARD_account_hpp
#define GUARD_account_hpp

#include "money.hpp"
#include "stored_object.hpp"
#include <string>

// Base class for accounts

class Account: public StoredObject
{
public:
private:
	Money m_balance;
	std::string m_name;
	std::string m_description;
};

// inline member functions



#endif
