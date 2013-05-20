#include "account_type.hpp"
#include "b_string.hpp"
#include "phatbooks_exceptions.hpp"
#include <jewel/debug_log.hpp>
#include <iostream>
#include <map>
#include <vector>

using std::cout;
using std::endl;
using std::map;
using std::vector;

namespace phatbooks
{


account_super_type::AccountSuperType
super_type(account_type::AccountType p_account_type)
{
	switch (p_account_type)
	{
	case account_type::asset:
	case account_type::liability:
	case account_type::equity:
		return account_super_type::balance_sheet;
	case account_type::revenue:
	case account_type::expense:
	case account_type::pure_envelope:
		return account_super_type::pl;
	default:
		assert (false);
	}
}

vector<account_type::AccountType>
account_types()
{
	vector<account_type::AccountType> ret;
	ret.reserve(6);
	ret.push_back(account_type::asset);
	ret.push_back(account_type::liability);
	ret.push_back(account_type::equity);
	ret.push_back(account_type::revenue);
	ret.push_back(account_type::expense);
	ret.push_back(account_type::pure_envelope);
	return ret;
}

vector<BString>
account_type_names()
{
	static bool calculated_already = false;
	static vector<BString> ret;
	while (!calculated_already)
	{
		ret.reserve(6);
		ret.push_back("Asset");
		ret.push_back("Liability");
		ret.push_back("Equity");
		ret.push_back("Revenue");
		ret.push_back("Expense");
		ret.push_back("Pure envelope");
		calculated_already = true;
	}
	return ret;
}


BString
account_type_to_string(account_type::AccountType p_account_type)
{
	size_t const index = static_cast<size_t>(p_account_type) - 1;
	return account_type_names()[index];
}


account_type::AccountType
string_to_account_type(BString const& p_string)
{
	static bool calculated_already = false;
	static map<BString, account_type::AccountType> dict;
	if (!calculated_already)
	{
		vector<BString> const names = account_type_names();
		int i = 1;
		for
		(	vector<BString>::const_iterator it = names.begin();
			it != names.end();
			++it, ++i
		)
		{
			dict[*it] = static_cast<account_type::AccountType>(i);
		}
		calculated_already = true;
	}
	map<BString, account_type::AccountType>::const_iterator jt =
		dict.find(p_string);
	if (jt == dict.end())
	{
		throw InvalidAccountTypeStringException
		(	"String does not identify AccountType"
		);
	}
	assert (jt != dict.end());
	return jt->second;
}


}  // namespace phatbooks
