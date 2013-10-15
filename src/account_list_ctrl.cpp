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


#include "gui/account_list_ctrl.hpp"
#include "account.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include "string_flags.hpp"
#include "gui/account_dialog.hpp"
#include "gui/app.hpp"
#include "gui/locale.hpp"
#include "gui/persistent_object_event.hpp"
#include "gui/top_panel.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/string.h>
#include <algorithm>
#include <set>

using boost::optional;
using sqloxx::Handle;
using std::max;
using std::set;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(AccountListCtrl, wxListCtrl)
	EVT_LIST_ITEM_ACTIVATED
	(	wxID_ANY,	
		AccountListCtrl::on_item_activated
	)
END_EVENT_TABLE()

AccountListCtrl::AccountListCtrl
(	wxWindow* p_parent,
	PhatbooksDatabaseConnection& p_database_connection,
	AccountSuperType p_account_super_type
):
	wxListCtrl
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		wxSize
		(	p_parent->GetClientSize().GetX() / 4,
			p_parent->GetClientSize().GetY()
		),
		wxLC_REPORT | wxFULL_REPAINT_ON_RESIZE
	),
	m_show_hidden(false),
	m_account_super_type(p_account_super_type),
	m_database_connection(p_database_connection)
{
	update();
}

AccountListCtrl::~AccountListCtrl()
{
}

void
AccountListCtrl::selected_accounts(set<sqloxx::Id>& out) const
{
	size_t i = 0;
	size_t const lim = GetItemCount();
	for ( ; i != lim; ++i)
	{
		if (GetItemState(i, wxLIST_STATE_SELECTED))
		{
			Handle<Account> const account
			(	m_database_connection,
				GetItemData(i)
			);
			JEWEL_ASSERT (account->has_id());
			out.insert(account->id());
		}
	}
	return;
}	

void
AccountListCtrl::on_item_activated(wxListEvent& event)
{
	sqloxx::Id const account_id = GetItemData(event.GetIndex());

	// Fire an Account editing request. This will be handled higher up
	// the window hierarchy.
	PersistentObjectEvent::fire
	(	this,
		PHATBOOKS_ACCOUNT_EDITING_EVENT,
		account_id
	);
	return;
 }

void
AccountListCtrl::update()
{
	// Remember which rows are selected currently
	set<sqloxx::Id> selected;
	selected_accounts(selected);

	// Now (re)draw
	ClearAll();
	InsertColumn
	(	s_name_col,
		account_concept_name
		(	m_account_super_type,
			AccountPhraseFlags().set(string_flags::capitalize)
		),
		wxLIST_FORMAT_LEFT
	);
	InsertColumn
	(	s_balance_col,
		wxString("Balance"),
		wxLIST_FORMAT_RIGHT
	);
	if (showing_daily_budget())
	{
		InsertColumn(s_budget_col, "Daily top-up", wxLIST_FORMAT_RIGHT);
	}

	long i = 0;  // because wxWidgets uses long
	AccountTableIterator it = make_type_name_ordered_account_table_iterator
	(	m_database_connection
	);
	AccountTableIterator const end;
	for ( ; it != end; ++it)
	{
		if
		(	(super_type((*it)->account_type()) == m_account_super_type) &&
			(m_show_hidden || ((*it)->visibility() == Visibility::visible))
		)	
		{
			// Insert item, with string for Column 0
			InsertItem(i, (*it)->name());
		
			JEWEL_ASSERT ((*it)->has_id());
			static_assert
			(	sizeof((*it)->id()) <= sizeof(i),
				"Object Id is too wide to be safely passed to "
				"SetItemData."
			);
			SetItemData(i, (*it)->id());

			// Insert the balance string
			SetItem
			(	i,
				s_balance_col,
				finformat_wx((*it)->friendly_balance(), locale())
			);

			if (showing_daily_budget())
			{
				// Insert budget string
				SetItem
				(	i,
					s_budget_col,
					finformat_wx((*it)->budget(), locale())
				);
			}

			++i;
		}
	}

	// Reinstate the selections we remembered
	for (size_t j = 0, lim = GetItemCount(); j != lim; ++j)
	{
		Handle<Account> const account
		(	m_database_connection,
			GetItemData(j)
		);
		JEWEL_ASSERT (account->has_id());
		if (selected.find(account->id()) != selected.end())
		{
			SetItemState
			(	j,
				wxLIST_STATE_SELECTED,
				wxLIST_STATE_SELECTED
			);
		}
	}

	// Configure column widths
	SetColumnWidth(s_name_col, wxLIST_AUTOSIZE_USEHEADER);
	SetColumnWidth(s_name_col, max(GetColumnWidth(s_name_col), 200));
	SetColumnWidth(s_balance_col, wxLIST_AUTOSIZE);
	SetColumnWidth(s_balance_col, max(GetColumnWidth(s_balance_col), 90));
	if (showing_daily_budget())
	{
		SetColumnWidth(s_budget_col, wxLIST_AUTOSIZE);
		SetColumnWidth(s_budget_col, max(GetColumnWidth(s_budget_col), 90));
	}

	Layout();

	return;
}

bool
AccountListCtrl::showing_daily_budget() const
{
	return m_account_super_type == AccountSuperType::pl;
}

optional<Handle<Account> >
AccountListCtrl::default_account() const
{
	optional<Handle<Account> > ret;
	if (GetItemCount() != 0)
	{
		JEWEL_ASSERT (GetItemCount() > 0);
		ret = Handle<Account>
		(	m_database_connection,
			GetItemData(GetTopItem())
		);
	}
	return ret;
}

void
AccountListCtrl::select_only(Handle<Account> const& p_account)
{
	JEWEL_ASSERT (p_account->has_id());  // precondition	

	size_t const sz = GetItemCount();	
	for (size_t i = 0; i != sz; ++i)
	{
		Handle<Account> const account(m_database_connection, GetItemData(i));
		long const filter = (wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
		long const flags = ((account == p_account)? filter: 0);
		SetItemState(i, flags, filter);
	}
	return;
}

bool
AccountListCtrl::toggle_showing_hidden()
{
	m_show_hidden = !m_show_hidden;
	update();
	return m_show_hidden;
}



}  // namespace gui
}  // namespace phatbooks
