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
	Account type m_account_type;
	std::string m_name;
	IdType commodity_id;
	std::string m_description;
	IdType id;
};


}  // namespace phatbooks


#endif  // GUARD_account_hpp
