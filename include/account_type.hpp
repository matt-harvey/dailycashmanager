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

#ifndef GUARD_account_type_hpp_5090496337549061
#define GUARD_account_type_hpp_5090496337549061

#include <wx/string.h>
#include <vector>

namespace dcm
{

/**
 * Each saved Account belongs to a certain AccountType.
 */
enum class AccountType: unsigned char
{
    // enum order is significant, as the database contains
    // a table with primary keys in this order.
    // Other functions in this file also depend on
    // the exact size and order of this enumeration.
    asset = 1,
    liability,
    equity,
    revenue,
    expense,
    pure_envelope,
};

/**
 * Each AccountType belongs to a certain AccountSuperType.
 */
enum class AccountSuperType: unsigned char
{
    balance_sheet,
    pl              // Profit and loss
};

AccountSuperType
super_type(AccountType p_account_type);

std::vector<AccountType> const&
account_types();

std::vector<AccountSuperType> const&
account_super_types();

std::vector<AccountType> const&
account_types(AccountSuperType p_account_super_type);

std::vector<wxString> const&
account_type_names();

wxString
account_type_to_string(AccountType p_account_type);

/**
 * @throws InvalidAccountTypeStringException if there is no
 * AccountType that corresponds to this string.
 */
AccountType
string_to_account_type(wxString const& p_string);

}  // namespace dcm

#endif  // GUARD_account_type_hpp_5090496337549061
