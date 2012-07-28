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

	/**
	 * Constructor
	 * Does not throw
	 */
	Account
	(	std::string p_name,
		AccountType p_account_type = profit_and_loss,
		std::string p_description = ""
	);

	// Default copy contructor is fine.
	// Default assignment is fine.
	// Default destructor is fine.

	/** Returns name of account.
	 */
	std::string name() const;

	/** Returns AccountType of account.
	 */
	AccountType account_type() const;

	/** Returns description of account.
	 */
	std::string description() const;

	
private:
	std::string m_name;
	AccountType m_account_type;
	std::string m_description;

	// native commodity or currency of Account
	std::string m_commodity_abbreviation;
};


}  // namespace phatbooks


#endif  // GUARD_account_hpp
