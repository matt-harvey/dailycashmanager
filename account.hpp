#ifndef GUARD_account_hpp
#define GUARD_account_hpp

#include "general_typedefs.hpp"
#include <string>

namespace phatbooks
{

class Account
{
public:
private:
	// enum order is significant, as the database contains
	// a table with primary keys in this order
	enum AccountType
	{
		profit_and_loss = 1,
		balance_sheet,
		envelope
	};
	std::string m_name;
	AccountType m_account_type;
	std::string m_description;

	// native commodity or currency of Account
	std::string m_commodity_abbreviation;
};


}  // namespace phatbooks


#endif  // GUARD_account_hpp
