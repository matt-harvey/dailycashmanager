#include "account.hpp"
#include <string>

using std::string;

namespace phatbooks
{

Account::Account
(	string p_name,
	AccountType p_account_type,
	string p_description
):
	m_name(p_name),
	m_account_type(p_account_type),
	m_description(p_description)
{
}
 
Account::AccountType
Account::account_type() const
{
	return m_account_type;
}

string Account::name() const
{
	return m_name;
}

string Account::description() const
{
	return m_description;
}



}  // namespace phatbooks
