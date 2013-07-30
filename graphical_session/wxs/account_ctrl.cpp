// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_ctrl.hpp"
#include "account.hpp"
#include "b_string.hpp"
#include "string_set_validator.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/combobox.h>
#include <wx/event.h>
#include <cassert>
#include <list>

using std::list;

// For debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(AccountCtrl, wxComboBox)
	EVT_KILL_FOCUS(AccountCtrl::on_kill_focus)
END_EVENT_TABLE()

namespace
{
	bool compare_account_names(Account const& lhs, Account const& rhs)
	{
		return bstring_to_wx(lhs.name()) < bstring_to_wx(rhs.name());
	}
}  // end anonymous namespace

Account
AccountCtrl::account()
{
	/*
	StringSetValidator const* const validator =
		dynamic_cast<StringSetValidator const*>(GetValidator());
	assert (validator);
	*/
	return Account(m_database_connection, m_account_map.at(GetValue()));
}

void
AccountCtrl::update_for_new(Account const& p_account)
{
	// TODO HIGH PRIORITY Implement this.
}

void
AccountCtrl::update_for_amended(Account const& p_account)
{
	Account const selected_account = account();
	list<Account> accounts;
	list<Account>::size_type const sz = GetCount();
	for (list<Account>::size_type i = 0; i != sz; ++i)
	{
		Account const acct
		(	m_database_connection,
			m_account_map.at(GetString(i))
		);
		accounts.push_back(acct);
	}
	accounts.sort(compare_account_names);
	set(accounts.begin(), accounts.end(), m_exclude_balancing_account);
	SetValue(bstring_to_wx(selected_account.name()));
	return;
}

void
AccountCtrl::on_kill_focus(wxFocusEvent& event)
{
	// Unfortunately if we call Validate() and TransferDataToWindow()
	// directly on the AccountCtrl, it doesn't work. We have to call
	// through parent instead.
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();
	event.Skip();
	return;
}


}  // namespace gui
}  // namespace phatbooks
