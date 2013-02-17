#include "account_type.hpp"
#include <wx/string.h>
#include <map>
#include <vector>

using std::map;
using std::vector;

namespace phatbooks
{




vector<wxString>
account_type_names()
{
	static bool calculated_already = false;
	static vector<wxString> ret;
	while (!calculated_already)
	{
		ret.push_back(L"Asset");
		ret.push_back(L"Liability");
		ret.push_back(L"Equity");
		ret.push_back(L"Revenue");
		ret.push_back(L"Expense");
		ret.push_back(L"Pure envelope");
		calculated_already = true;
	}
	return ret;
}


wxString
account_type_to_string(account_type::AccountType p_account_type)
{
	size_t const index = static_cast<size_t>(p_account_type) - 1;
	return account_type_names()[index];
}


account_type::AccountType
string_to_account_type(wxString const& p_string)
{
	static bool calculated_already = false;
	static map<wxString, account_type::AccountType> dict;
	if (!calculated_already)
	{
		vector<wxString> const names = account_type_names();
		int i = 1;
		for
		(	vector<wxString>::const_iterator it = names.begin();
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
