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

#include "gui/account_type_ctrl.hpp"
#include "account_type.hpp"
#include "phatbooks_exceptions.hpp"
#include "gui/account_dialog.hpp"
#include "gui/combo_box.hpp"
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <wx/combobox.h>
#include <wx/string.h>
#include <wx/window.h>
#include <vector>

using std::vector;

namespace phatbooks
{
namespace gui
{

namespace
{
	wxString default_account_type_string
	(	AccountSuperType p_account_super_type
	)
	{
		JEWEL_ASSERT (!account_types(p_account_super_type).empty());
		AccountType default_account_type =
			account_types(p_account_super_type)[0];
		if (default_account_type == AccountType::equity)
		{
			JEWEL_ASSERT (account_types(p_account_super_type).size() > 1);
			default_account_type = account_types(p_account_super_type)[1];
		}
		return account_type_to_string(default_account_type);
	}

}  // end anonymous namespace


AccountTypeCtrl::AccountTypeCtrl
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	AccountSuperType p_account_super_type
):
	ComboBox
	(	p_parent,
		p_id,
		default_account_type_string(p_account_super_type),
		wxDefaultPosition,
		p_size,
		wxArrayString(),
		wxCB_READONLY | wxTAB_TRAVERSAL
	),
	m_account_super_type(p_account_super_type),
	m_database_connection(p_database_connection)
{
	typedef vector<AccountType> ATypeVec;
	ATypeVec const& atypes = account_types(m_account_super_type);
	for (AccountType const elem: atypes)
	{
		// HACK to stop users from accessing AccountType::equity.
		if (elem != AccountType::equity)
		{
			Append(account_type_to_string(elem));
		}
	}	
	SetSelection(0);  // In effort to avoid apparent bug in Windows
}

AccountType
AccountTypeCtrl::account_type() const
{
	JEWEL_ASSERT (!GetValue().IsEmpty());
	return string_to_account_type(GetValue());
}

void
AccountTypeCtrl::set_account_type(AccountType p_account_type)
{
	if (super_type(p_account_type) != m_account_super_type)
	{
		JEWEL_THROW
		(	InvalidAccountTypeException,
			"AccountType is invalid given the AccountSuperType of this "
			"AccountTypeCtrl."
		);
	}
	JEWEL_ASSERT (m_account_super_type == super_type(p_account_type));
	SetValue(account_type_to_string(p_account_type));
	return;
}


}  // namespace gui
}  // namespace phatbooks
