/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_account_type_hpp_5090496337549061
#define GUARD_account_type_hpp_5090496337549061

#include <wx/string.h>
#include <vector>

namespace phatbooks
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
	pl  			// Profit and loss
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



}  // namespace phatbooks

#endif  // GUARD_account_type_hpp_5090496337549061


