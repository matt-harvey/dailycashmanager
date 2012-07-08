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
	enum AccountType
	{
		balance_sheet,
		profit_and_loss,
		envelope
	};
	AccountType m_account_type;
	std::string m_name;
	IdType m_commodity_id;  // native commodity or currency of Account
	std::string m_description;
	IdType m_id;
};


}  // namespace phatbooks


#endif  // GUARD_account_hpp
