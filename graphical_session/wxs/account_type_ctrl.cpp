// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_type_ctrl.hpp"
#include "account_dialog.hpp"
#include "account_type.hpp"
#include "phatbooks_exceptions.hpp"
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
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		assert (!account_types(p_account_super_type).empty());
		account_type::AccountType default_account_type =
			account_types(p_account_super_type)[0];
		if (default_account_type == account_type::equity)
		{
			assert (account_types(p_account_super_type).size() > 1);
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
	account_super_type::AccountSuperType p_account_super_type
):
	wxComboBox
	(	p_parent,
		p_id,
		default_account_type_string(p_account_super_type),
		wxDefaultPosition,
		p_size,
		wxArrayString(),
		wxCB_READONLY
	),
	m_account_super_type(p_account_super_type),
	m_database_connection(p_database_connection)
{
	typedef vector<account_type::AccountType> ATypeVec;
	ATypeVec const& atypes = account_types(m_account_super_type);
	ATypeVec::const_iterator it = atypes.begin();
	ATypeVec::const_iterator const end = atypes.end();
	for ( ; it != end; ++it)
	{
		// WARNING Hack to stop users from accessing account_type::equity.
		if (*it != account_type::equity)
		{
			Append(account_type_to_string(*it));
		}
	}	
	SetSelection(0);  // In effort to avoid apparent bug in Windows
}

account_type::AccountType
AccountTypeCtrl::account_type() const
{
	assert (!GetValue().IsEmpty());
	return string_to_account_type(GetValue());
}

void
AccountTypeCtrl::set_account_type(account_type::AccountType p_account_type)
{
	if (super_type(p_account_type) != m_account_super_type)
	{
		throw InvalidAccountTypeException
		(	"AccountType is invalid given the AccountSuperType of this "
			"AccountTypeCtrl."
		);
	}
	assert (m_account_super_type == super_type(p_account_type));
	SetValue(account_type_to_string(p_account_type));
	return;
}


}  // namespace gui
}  // namespace phatbooks
