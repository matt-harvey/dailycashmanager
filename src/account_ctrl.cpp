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

#include "gui/account_ctrl.hpp"
#include "account.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "dcm_database_connection.hpp"
#include "dcm_exceptions.hpp"
#include "string_flags.hpp"
#include "gui/combo_box.hpp"
#include "gui/string_set_validator.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <wx/event.h>
#include <wx/string.h>
#include <set>
#include <sstream>
#include <vector>

using jewel::Log;
using sqloxx::Handle;
using sqloxx::Id;
using std::ostringstream;
using std::set;
using std::vector;

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(AccountCtrl, ComboBox)
	EVT_KILL_FOCUS(AccountCtrl::on_kill_focus)
END_EVENT_TABLE()

namespace
{
	bool compare_account_names
	(	Handle<Account> const& lhs,
		Handle<Account> const& rhs
	)
	{
		return lhs->name() < rhs->name();
	}
}  // end anonymous namespace

AccountCtrl::AccountCtrl
(	wxWindow* p_parent,
	unsigned int p_id,
	wxSize const& p_size,
	vector<AccountType> const& p_account_types,
	DcmDatabaseConnection& p_database_connection,
	bool p_exclude_balancing_account
):
	ComboBox
	(	p_parent,
		p_id,
		wxEmptyString,
		wxDefaultPosition,
		p_size,
		wxArrayString(),	
		wxCB_SORT
	),
	m_database_connection(p_database_connection)
{
	JEWEL_LOG_TRACE();
	JEWEL_ASSERT (m_available_account_types.empty());
	reset_for_account_types(p_account_types, p_exclude_balancing_account);
	JEWEL_ASSERT (m_exclude_balancing_account == p_exclude_balancing_account);
}

AccountCtrl::~AccountCtrl()
{
}

void
AccountCtrl::reset_for_account_types
(	vector<AccountType> const& p_account_types,
	bool p_exclude_balancing_account
)
{
	JEWEL_LOG_TRACE();
	if (p_account_types.empty())
	{
		JEWEL_THROW
		(	InvalidAccountTypeException,
			"Attempted to configure AccountCtrl with empty vector<AccountType>."
		);
	}
	m_available_account_types = set<AccountType>
	(	p_account_types.begin(),
		p_account_types.end()
	);
	m_exclude_balancing_account = p_exclude_balancing_account;
	reset();
}

void
AccountCtrl::reset(Handle<Account> const& p_preserved_account)
{
	// TODO LOW PRIORITY Tidy this up a bit.
	JEWEL_LOG_TRACE();
	Handle<Account> const balancing_acct =
		m_database_connection.balancing_account();
	if (m_exclude_balancing_account && (p_preserved_account == balancing_acct))
	{
		JEWEL_THROW
		(	InvalidAccountException,
			"Attempted to set AccountCtrl to the balancing Account when this "
				"AccountCtrl has been configured to exclude the balancing "
				"account."
		);
	}
	if
	(	p_preserved_account &&
		!supports_account_type(p_preserved_account->account_type())
	)
	{
		JEWEL_THROW
		(	InvalidAccountException,
			"Attempted to set AccountCtrl to the an Account of an AccountType "
				"that is not supported by the AccountCtrl."
		);
	}	
	m_account_map.clear();
	wxArrayString valid_account_names;
	AccountTableIterator it(m_database_connection);
	AccountTableIterator const end;
	for ( ; it != end; ++it)
	{
		Handle<Account> const& acc = *it;
		if (supports_account_type(acc->account_type()))
		{
			if (m_exclude_balancing_account && (acc == balancing_acct))
			{
				// Then don't include it
			}
			else if
			(	(acc->visibility() == Visibility::visible) ||
				(acc == p_preserved_account)
			)
			{
				wxString const name = acc->name();
				valid_account_names.Add(name);

				// Remember the Account associated with this name (comes
				// in handy when we have to update for a change in Account
				// name).
				JEWEL_ASSERT (acc->has_id());
				m_account_map[name] = acc->id();
			}
		}
		else
		{
			JEWEL_ASSERT (acc != p_preserved_account);
		}
	}
	if (m_account_map.empty())
	{
		JEWEL_THROW
		(	InvalidAccountException,
			"There are no saved Accounts with AccountTypes supported by "
			"this AccountCtrl."
		);
	}
	JEWEL_ASSERT (!valid_account_names.IsEmpty());
	StringSetValidator validator
	(	valid_account_names[0],
		valid_account_names,
		account_concepts_phrase()
	);
	Set(valid_account_names);
	SetValidator(validator);
	AutoComplete(valid_account_names);
	SetValue(valid_account_names[0]);
	return;
}

void
AccountCtrl::set_account(Handle<Account> const& p_account)
{
	wxString const name = p_account->name();
	JEWEL_LOG_VALUE(Log::info, name);
	if (m_account_map.find(name) == m_account_map.end()) reset(p_account);
	JEWEL_ASSERT
	(	!m_exclude_balancing_account || 
		(p_account != m_database_connection.balancing_account())
	);
	JEWEL_ASSERT (supports_account_type(p_account->account_type()));
	StringSetValidator* const validator =
		dynamic_cast<StringSetValidator*>(GetValidator());
	JEWEL_ASSERT (validator);
	SetValue(name);
	validator->TransferFromWindow();
	JEWEL_LOG_TRACE();
	return;
}

Handle<Account>
AccountCtrl::account()
{
	JEWEL_LOG_TRACE();
	// Note it may NOT necessarily be the case at this point that
	// GetValue() == validator->m_text.
	wxString const account_name = GetValue();
	JEWEL_LOG_VALUE(Log::info, account_name);
	Id const account_id = m_account_map.at(account_name);
	auto const ret = Handle<Account>(m_database_connection, account_id);
	JEWEL_LOG_TRACE();
	return ret;
}

void
AccountCtrl::update_for_new(Handle<Account> const& p_account)
{
	JEWEL_LOG_TRACE();
	(void)p_account;  // silence compiler re. unused parameter
	refresh();
	return;
}

void
AccountCtrl::update_for_amended(Handle<Account> const& p_account)
{
	JEWEL_LOG_TRACE();
	(void)p_account;  // silence compiler re. unused parameter
	refresh();
	return;
}

void
AccountCtrl::on_kill_focus(wxFocusEvent& event)
{
	JEWEL_LOG_TRACE();
	StringSetValidator* const validator =
		dynamic_cast<StringSetValidator*>(GetValidator());
	JEWEL_ASSERT (validator);
	validator->Validate(this);
	validator->TransferToWindow();
	event.Skip();
	return;
}

bool
AccountCtrl::supports_account_type(AccountType p_account_type)
{
	return
		m_available_account_types.find(p_account_type) !=
		m_available_account_types.end();
}

void
AccountCtrl::refresh()
{
	JEWEL_LOG_TRACE();
	Handle<Account> const selected_account = account();
	JEWEL_LOG_VALUE(Log::info, selected_account->name());
	reset();
	set_account(selected_account);
	return;
}

}  // namespace gui
}  // namespace dcm
