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
#include "dcm_exceptions.hpp"
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

namespace dcm
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
    static vector<AccountType> const ret
    {   AccountType::asset,
        AccountType::liability,
        AccountType::equity,
        AccountType::revenue,
        AccountType::expense,
        AccountType::pure_envelope
    };
    return ret;
}

vector<AccountType> const&
account_types(AccountSuperType p_account_super_type, bool p_expense_first)
{
    static vector<AccountType> const pl_ret_expense_first
    {   AccountType::expense,
        AccountType::revenue,
        AccountType::pure_envelope
    };
    static vector<AccountType> const pl_ret_logical
    {   AccountType::revenue,
        AccountType::expense,
        AccountType::pure_envelope
    };
    static vector<AccountType> const bs_ret
    {   AccountType::asset,
        AccountType::liability,
        AccountType::equity
    };
    switch (p_account_super_type)
    {
    case AccountSuperType::pl:
        return p_expense_first ? pl_ret_expense_first : pl_ret_logical;
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
    static vector<AccountSuperType> const ret
    {   AccountSuperType::balance_sheet,
        AccountSuperType::pl
    };
    return ret;
}

vector<wxString> const&
account_type_names()
{
    static vector<wxString> const ret
    {   "Asset",
        "Liability",
        "Equity",
        "Revenue",
        "Expense",
        "Pure envelope"
    };
    JEWEL_ASSERT (ret.size() == account_types().size());
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
        (   InvalidAccountTypeStringException,
            "String does not identify AccountType"
        );
    }
    JEWEL_ASSERT (jt != dict.end());
    return jt->second;
}

}  // namespace dcm
