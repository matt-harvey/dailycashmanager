/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "account_type.hpp"
#include "phatbooks_exceptions.hpp"
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <wx/string.h>
#include <iostream>
#include <map>
#include <vector>

using std::cout;
using std::endl;
using std::map;
using std::vector;

namespace phatbooks
{

AccountSuperType
super_type(AccountType p_account_type)
{
	switch (p_account_type)
	{
	case AccountType::asset:
	case AccountType::liability:
	case AccountType::equity:
		return AccountSuperType::balance_sheet;
	case AccountType::revenue:
	case AccountType::expense:
	case AccountType::pure_envelope:
		return AccountSuperType::pl;
	default:
		JEWEL_HARD_ASSERT (false);
	}
}

vector<AccountType> const&
account_types()
{
	static vector<AccountType> ret;
	if (ret.empty())
	{
		ret.reserve(6);
		ret.push_back(AccountType::asset);
		ret.push_back(AccountType::liability);
		ret.push_back(AccountType::equity);
		ret.push_back(AccountType::revenue);
		ret.push_back(AccountType::expense);
		ret.push_back(AccountType::pure_envelope);
	}
	JEWEL_ASSERT (ret.size() == 6);
	return ret;
}

vector<AccountType> const&
account_types(AccountSuperType p_account_super_type)
{
	static vector<AccountType> pl_ret;
	static vector<AccountType> bs_ret;
	if (pl_ret.empty())
	{
		pl_ret.reserve(3);
		pl_ret.push_back(AccountType::revenue);
		pl_ret.push_back(AccountType::expense);
		pl_ret.push_back(AccountType::pure_envelope);
	}
	if (bs_ret.empty())
	{
		bs_ret.reserve(3);
		bs_ret.push_back(AccountType::asset);
		bs_ret.push_back(AccountType::liability);
		bs_ret.push_back(AccountType::equity);
	}
	switch (p_account_super_type)
	{
	case AccountSuperType::pl:
		return pl_ret;
	case AccountSuperType::balance_sheet:
		return bs_ret;
	default:
		JEWEL_HARD_ASSERT (false);
	}
	JEWEL_HARD_ASSERT (false);
}

vector<AccountSuperType> const&
account_super_types()
{
	static vector<AccountSuperType> ret;
	if (ret.empty())
	{
		ret.push_back(AccountSuperType::balance_sheet);
		ret.push_back(AccountSuperType::pl);
	}
	JEWEL_ASSERT (!ret.empty());
	return ret;
}

vector<wxString> const&
account_type_names()
{
	static vector<wxString> ret;
	if (ret.empty())
	{
		ret.reserve(6);
		ret.push_back("Asset");
		ret.push_back("Liability");
		ret.push_back("Equity");
		ret.push_back("Revenue");
		ret.push_back("Expense");
		ret.push_back("Pure envelope");
	}
	JEWEL_ASSERT (ret.size() == 6);
	return ret;
}

wxString
account_type_to_string(AccountType p_account_type)
{
	size_t const index = static_cast<size_t>(p_account_type) - 1;
	return account_type_names()[index];
}

AccountType
string_to_account_type(wxString const& p_string)
{
	static bool calculated_already = false;
	static map<wxString, AccountType> dict;
	if (!calculated_already)
	{
		vector<wxString> const names = account_type_names();
		int i = 1;
		for (auto const& name: names)
		{
			dict[name] = static_cast<AccountType>(i);
			++i;
		}
		calculated_already = true;
	}
	auto const jt = dict.find(p_string);
	if (jt == dict.end())
	{
		JEWEL_THROW
		(	InvalidAccountTypeStringException,
			"String does not identify AccountType"
		);
	}
	JEWEL_ASSERT (jt != dict.end());
	return jt->second;
}

}  // namespace phatbooks
