#include "account.hpp"
#include <string>

using std::string;

namespace phatbooks
{

Account::Account
(	AccountType p_account_type,
	string p_name,
	string p_description
):
	m_account_type(p_account_type),
	m_name(p_name),
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
