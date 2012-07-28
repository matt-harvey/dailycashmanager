#ifndef GUARD_account_hpp
#define GUARD_account_hpp

#include "general_typedefs.hpp"
#include <string>

namespace phatbooks
{

class Account
{
public:

	enum AccountType
	{
		// enum order is significant, as the database contains
		// a table with primary keys in this order
		profit_and_loss = 1,
		balance_sheet,
		pure_envelope
	};

	AccountType account_type() const;
	
	std::string name() const;

	std::string description() const;

	/**
	 * Constructor
	 * Does not throw
	 */
	Account(AccountType p_account_type = profit_and_loss,
	  std::string p_name = "", std::string p_description = "");

	// Default destructor suffices
	
private:
	AccountType m_account_type;
	std::string m_name;
	std::string m_description;

	// native commodity or currency of Account
	std::string m_commodity_abbreviation;
};


}  // namespace phatbooks


#endif  // GUARD_account_hpp
