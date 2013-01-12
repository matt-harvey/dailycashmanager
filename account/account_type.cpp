#include "account_type.hpp"
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

namespace phatbooks
{




vector<string>
account_type_names()
{
	static bool calculated_already = false;
	static vector<string> ret;
	while (!calculated_already)
	{
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


string
account_type_to_string(account_type::AccountType p_account_type)
{
	size_t const index = static_cast<size_t>(p_account_type) - 1;
	return account_type_names()[index];
}


account_type::AccountType
string_to_account_type(std::string const& p_string)
{
	static bool calculated_already = false;
	static map<string, account_type::AccountType> dict;
	if (!calculated_already)
	{
		vector<string> const names = account_type_names();
		int i = 1;
		for
		(	vector<string>::const_iterator it = names.begin();
			it != names.end();
			++it, ++i
		)
		{
			dict[*it] = static_cast<account_type::AccountType>(i);
		}
		calculated_already = true;
	}
	return dict[p_string];
}



}  // namespace phatbooks
