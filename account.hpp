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
	BalanceType balance() const;
	Commodity commodity() const;
	std::string name() const;
	std::string description() const;
	void set_balance(BalanceType);
	void set_name(std::string const&);
	void set_description(std::string const&);
private:
	Commodity commodity_;
	BalanceType balance_;
	std::string name_;
	std::string description_;
};

// inline member functions


inline Account::BalanceType Account::balance() const
{
	return balance_;
}

inline Commodity Account::commodity() const
{
	return commodity_;
}

inline std::string Account::name() const
{
	return name_;
}

inline std::string Account::description() const
{
	return description_;
}

inline void Account::set_balance(Account::BalanceType new_balance)
{
	balance_ = new_balance;
	return;
}

inline void Account::set_name(std::string const& new_name)
{
	name_ = new_name;
	return;
}

inline void Account::set_description(std::string const& new_description)
{
	description_ = new_description;
	return;
}




#endif
