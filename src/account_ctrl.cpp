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
#include "string_flags.hpp"
#include "gui/string_set_validator.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <sqloxx/handle.hpp>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/string.h>
#include <set>
#include <vector>

using jewel::Log;
using sqloxx::Handle;
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
	JEWEL_ASSERT (m_available_account_types.empty());
	reset(p_account_types, p_exclude_balancing_account);
	JEWEL_ASSERT (m_exclude_balancing_account == p_exclude_balancing_account);
}

AccountCtrl::~AccountCtrl()
{
}

void
AccountCtrl::reset
(	vector<AccountType> const& p_account_types,
	bool p_exclude_balancing_account
)
{
	m_available_account_types = set<AccountType>
	(	p_account_types.begin(),
		p_account_types.end()
	);
	m_exclude_balancing_account = p_exclude_balancing_account;
	reset();
}

void
AccountCtrl::reset()
{
	m_account_map.clear();
	wxArrayString valid_account_names;
	Handle<Account> const balancing_acct =
		m_database_connection.balancing_account();
	vector<Handle<Account> > avec
	(	AccountTableIterator(m_database_connection),
		(AccountTableIterator())
	);
	AccountTableIterator it(m_database_connection);
	AccountTableIterator const end;
	for ( ; it != end; ++it)
	{
		if
		(	m_available_account_types.find((*it)->account_type()) !=
			m_available_account_types.end()
		)
		{
			if (m_exclude_balancing_account && (*it == balancing_acct))
			{
				// Then don't include it
			}
			else
			{
				valid_account_names.Add((*it)->name());

				// Remember the Account associated with this name (comes
				// in handy when we have to update for a change in Account
				// name).
				m_account_map[(*it)->name()] = (*it)->id();
			}
		}
	}
	JEWEL_ASSERT (!valid_account_names.IsEmpty());  // TODO HIGH PRIORITY Can this fail?
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
	JEWEL_LOG_VALUE(Log::info, p_account->name());
	StringSetValidator* const validator =
		dynamic_cast<StringSetValidator*>(GetValidator());
	SetValue(p_account->name());
	validator->TransferFromWindow();
	return;
}

Handle<Account>
AccountCtrl::account()
{
	// Note it may NOT necessarily be the case at this point that
	// GetValue() == validator->m_text.
	return Handle<Account>
	(	m_database_connection,
		m_account_map.at(GetValue())
	);
}

void
AccountCtrl::update_for_new(Handle<Account> const& p_account)
{
	(void)p_account;  // silence compiler re. unused parameter
	refresh();
	return;
}

void
AccountCtrl::update_for_amended(Handle<Account> const& p_account)
{
	(void)p_account;  // silence compiler re. unused parameter
	refresh();
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
