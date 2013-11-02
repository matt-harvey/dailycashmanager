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


#include "gui/account_ctrl.hpp"
#include "account.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "string_flags.hpp"
#include "gui/string_set_validator.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <wx/combobox.h>
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

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(AccountCtrl, wxComboBox)
	EVT_KILL_FOCUS(AccountCtrl::on_kill_focus)
END_EVENT_TABLE()

namespace
{
	bool
	compare_account_names(Handle<Account> const& lhs, Handle<Account> const& rhs)
	{
		return lhs->name() < rhs->name();
	}
}  // end anonymous namespace

AccountCtrl::AccountCtrl
(	wxWindow* p_parent,
	unsigned int p_id,
	wxSize const& p_size,
	vector<AccountType> const& p_account_types,
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
	m_account_map.clear();
	wxArrayString valid_account_names;
	Handle<Account> const balancing_acct =
		m_database_connection.balancing_account();
	AccountTableIterator it(m_database_connection);
	AccountTableIterator const end;
	for ( ; it != end; ++it)
	{
		Handle<Account> const& acc = *it;
		if
		(	(	m_available_account_types.find(acc->account_type()) !=
				m_available_account_types.end()
			)	||
			(	acc == p_preserved_account
			)
		)
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
	if (m_account_map.find(name) == m_account_map.end())
	{
		reset(p_account);
		/*
		ostringstream oss;
		oss << "Account with name \"" << name << "\" is not supported by this "
			<< "AccountCtrl.";
		char const* msg = oss.str().c_str();
		JEWEL_THROW(InvalidAccountException, msg);
		*/
	}
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
	// Unfortunately if we call Validate() and TransferDataToWindow()
	// directly on the AccountCtrl, it doesn't work. We have to call
	// through parent instead.
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();
	event.Skip();
	return;
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
}  // namespace phatbooks
