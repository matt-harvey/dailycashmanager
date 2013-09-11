// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_list_ctrl.hpp"
#include "app.hpp"
#include "account.hpp"
#include "account_dialog.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "top_panel.hpp"
#include "persistent_object_event.hpp"
#include "phatbooks_database_connection.hpp"
#include "string_flags.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/string.h>
#include <algorithm>
#include <set>

using boost::optional;
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

AccountListCtrl*
AccountListCtrl::create_balance_sheet_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	AccountListCtrl* ret = new AccountListCtrl
	(	parent,
		dbc,
		account_super_type::balance_sheet
	);
	return ret;
}

AccountListCtrl*
AccountListCtrl::create_pl_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	AccountListCtrl* ret = new AccountListCtrl
	(	parent,
		dbc,
		account_super_type::pl
	);
	return ret;
}

AccountListCtrl::AccountListCtrl
(	wxWindow* p_parent,
	PhatbooksDatabaseConnection& p_database_connection,
	account_super_type::AccountSuperType p_account_super_type
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

void
AccountListCtrl::selected_accounts(set<Account::Id>& out) const
{
	size_t i = 0;
	size_t const lim = GetItemCount();
	for ( ; i != lim; ++i)
	{
		if (GetItemState(i, wxLIST_STATE_SELECTED))
		{
			Account const account
			(	m_database_connection,
				GetItemData(i)
			);
			JEWEL_ASSERT (account.has_id());
			out.insert(account.id());
		}
	}
	return;
}	

void
AccountListCtrl::on_item_activated(wxListEvent& event)
{
	Account account(m_database_connection, GetItemData(event.GetIndex()));

	// Fire an Account editing request. This will be handled higher up
	// the window hierarchy.
	PersistentObjectEvent::fire
	(	this,
		PHATBOOKS_ACCOUNT_EDITING_EVENT,
		account
	);
	return;
 }

void
AccountListCtrl::update()
{
	// Remember which rows are selected currently
	set<Account::Id> selected;
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

	size_t i = 0;
	AccountTableIterator it = make_type_name_ordered_account_table_iterator
	(	m_database_connection
	);
	AccountTableIterator const end;
	for ( ; it != end; ++it)
	{
		if
		(	(super_type(it->account_type()) == m_account_super_type) &&
			(m_show_hidden || (it->visibility() == visibility::visible))
		)	
		{
			// Insert item, with string for Column 0
			InsertItem(i, it->name());
		
			// TODO Do a static assert to ensure second param will fit the id.
			JEWEL_ASSERT (it->has_id());
			SetItemData(i, it->id());

			// Insert the balance string
			SetItem
			(	i,
				s_balance_col,
				finformat_wx(it->friendly_balance(), locale())
			);

			if (showing_daily_budget())
			{
				// Insert budget string
				SetItem
				(	i,
					s_budget_col,
					finformat_wx(it->budget(), locale())
				);
			}

			++i;
		}
	}

	// Reinstate the selections we remembered
	for (size_t j = 0, lim = GetItemCount(); j != lim; ++j)
	{
		Account const account
		(	m_database_connection,
			GetItemData(j)
		);
		JEWEL_ASSERT (account.has_id());
		if (selected.find(account.id()) != selected.end())
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
	return m_account_super_type == account_super_type::pl;
}

optional<Account>
AccountListCtrl::default_account() const
{
	optional<Account> ret;
	if (GetItemCount() != 0)
	{
		JEWEL_ASSERT (GetItemCount() > 0);
		ret = Account
		(	m_database_connection,
			GetItemData(GetTopItem())
		);
	}
	return ret;
}

void
AccountListCtrl::select_only(Account const& p_account)
{
	JEWEL_ASSERT (p_account.has_id());  // precondition	

	size_t const sz = GetItemCount();	
	for (size_t i = 0; i != sz; ++i)
	{
		Account const account(m_database_connection, GetItemData(i));
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
