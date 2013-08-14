// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_ctrl.hpp"
#include "account.hpp"
#include "account_reader.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "string_set_validator.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/combobox.h>
#include <wx/event.h>
#include <cassert>
#include <set>

using std::set;
using std::vector;

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


AccountCtrl::AccountCtrl
(	wxWindow* p_parent,
	unsigned int p_id,
	wxSize const& p_size,
	vector<account_type::AccountType> const& p_account_types,
	PhatbooksDatabaseConnection& p_database_connection,
	bool p_exclude_balancing_account
):
	wxComboBox
	(	p_parent,
		p_id,
		wxEmptyString,
		wxDefaultPosition,
		p_size,
		wxArrayString(),	
		wxCB_SORT
	),
	m_exclude_balancing_account(p_exclude_balancing_account),
	m_database_connection(p_database_connection)
{
	// TODO Factor out stuff that's common between this and AccountCtrl::reset(...).
	using account_type::AccountType;
	wxArrayString valid_account_names;
	assert (valid_account_names.IsEmpty());
	Account const balancing_account =
		m_database_connection.balancing_account();
	AccountReader reader(m_database_connection);
	vector<AccountType>::const_iterator atit = p_account_types.begin();
	vector<AccountType>::const_iterator const atend = p_account_types.end();
	for ( ; atit != atend; ++atit)
	{
		AccountReader::const_iterator arit = reader.begin();
		AccountReader::const_iterator const arend = reader.end();
		for ( ; arit != arend; ++arit)
		{
			if (arit->account_type() == *atit)
			{
				if (m_exclude_balancing_account && *arit == balancing_account)
				{
					// Then don't include it.
				}
				else
				{
					wxString const name_wx = bstring_to_wx(arit->name());
					valid_account_names.Add(name_wx);  // remembers as valid
					Append(name_wx);  // adds to combobox

					// Remember the Account associated with this name (comes
					// in handy when we have to update for a change in Account
					// name).
					m_account_map[name_wx] = arit->id();
				}
			}
		}
	}
	assert (!reader.empty());  // TODO What if this fails?
	StringSetValidator validator
	(	bstring_to_wx(reader.begin()->name()),
		valid_account_names,
		bstring_to_wx(account_concepts_phrase())
	);
	SetValidator(validator);
	AutoComplete(valid_account_names);
	assert (!valid_account_names.IsEmpty());
	SetValue(valid_account_names[0]);
}


void
AccountCtrl::reset
(	vector<account_type::AccountType> const& p_account_types,
	bool p_exclude_balancing_account
)
{
	using account_type::AccountType;
	using account_type::AccountType;
	m_account_map.clear();
	m_exclude_balancing_account = p_exclude_balancing_account;
	wxArrayString valid_account_names;
	Account const balancing_account =
		m_database_connection.balancing_account();
	AccountReader reader(m_database_connection);
	vector<AccountType>::const_iterator atit = p_account_types.begin();
	vector<AccountType>::const_iterator const atend = p_account_types.end();
	for ( ; atit != atend; ++atit)
	{
		AccountReader::const_iterator arit = reader.begin();
		AccountReader::const_iterator const arend = reader.end();
		for ( ; arit != arend; ++arit)
		{
			if (arit->account_type() == *atit)
			{
				if (m_exclude_balancing_account && *arit == balancing_account)
				{
					// Then don't include it
				}
				else
				{
					wxString const name_wx = bstring_to_wx(arit->name());
					valid_account_names.Add(name_wx);
					m_account_map[name_wx] = arit->id();
				}
			}
		}
	}
	assert (!valid_account_names.IsEmpty());  // TODO Can this fail?
	StringSetValidator validator
	(	valid_account_names[0],
		valid_account_names,
		bstring_to_wx(account_concepts_phrase())
	);
	Set(valid_account_names);
	SetValidator(validator);
	AutoComplete(valid_account_names);
	SetValue(valid_account_names[0]);
	return;
}

void
AccountCtrl::set_account(Account const& p_account)
{
	SetValue(bstring_to_wx(p_account.name()));
	return;
}

Account
AccountCtrl::account()
{
	/*
	StringSetValidator const* const validator =
		dynamic_cast<StringSetValidator const*>(GetValidator());
	assert (validator);
	*/
	JEWEL_DEBUG_LOG << "AccountCtrl::GetValue(): " << GetValue() << endl;
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
	// WARNING This is clunky and inefficient
	(void)p_account;  // silence compiler re. unused parameter
	using account_type::AccountType;
	Account const selected_account = account();
	size_t i = 0;
	size_t const sz = GetCount();
	set<AccountType> atypes;
	for ( ; i != sz; ++i)
	{
		Account const acct
		(	m_database_connection,
			m_account_map.at(GetString(i))
		);
		atypes.insert(acct.account_type());
	}
	vector<AccountType> atypes_vec(atypes.begin(), atypes.end());
	reset(atypes_vec, m_exclude_balancing_account);
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
