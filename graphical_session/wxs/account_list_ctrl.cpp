// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_list_ctrl.hpp"
#include "app.hpp"
#include "account.hpp"
#include "account_reader.hpp"
#include "b_string.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/optional.hpp>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/wx.h>
#include <algorithm>
#include <cassert>
#include <vector>

using boost::optional;
using std::max;
using std::vector;


namespace phatbooks
{
namespace gui
{


AccountListCtrl*
AccountListCtrl::create_balance_sheet_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	BalanceSheetAccountReader const reader(dbc);
	AccountListCtrl* ret = new AccountListCtrl
	(	parent,
		reader,
		dbc,
		false,
		wxString("Account")
	);
	return ret;
}

AccountListCtrl*
AccountListCtrl::create_pl_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	PLAccountReader const reader(dbc);
	AccountListCtrl* ret = new AccountListCtrl
	(	parent,
		reader,
		dbc,
		true,
		wxString("Category")
	);
	return ret;
}

AccountListCtrl::AccountListCtrl
(	wxWindow* p_parent,
	AccountReaderBase const& p_reader,
	PhatbooksDatabaseConnection& p_database_connection,
	bool p_show_daily_budget,
	wxString const& p_left_column_title
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
	m_show_daily_budget(p_show_daily_budget),
	m_database_connection(p_database_connection)
{
	update(p_reader, p_left_column_title);
}

void
AccountListCtrl::selected_accounts(vector<Account>& out)
{
	size_t i = 0;
	size_t const lim = GetItemCount();
	for ( ; i != lim; ++i)
	{
		if(GetItemState(i, wxLIST_STATE_SELECTED))
		{
			Account const account
			(	m_database_connection,
				wx_to_bstring(GetItemText(i))
			);
			out.push_back(account);
		}
	}
	return;
}	

void
AccountListCtrl::update(bool balance_sheet)
{
	if (balance_sheet)
	{
		BalanceSheetAccountReader const reader(m_database_connection);
		update(reader, "Account");
	}
	else
	{
		PLAccountReader const reader(m_database_connection);
		update(reader, "Category");
	}
	return;
}

void
AccountListCtrl::update
(	AccountReaderBase const& p_reader,
	wxString const& p_left_column_title
)
{
	// Remember which rows are selected currently
	vector<size_t> selected_rows;
	size_t const lim = GetItemCount();
	for (size_t j = 0 ; j != lim; ++j)
	{
		if (GetItemState(j, wxLIST_STATE_SELECTED))
		{
			selected_rows.push_back(j);
		}
	}

	// Now (re)draw
	ClearAll();
	InsertColumn(s_name_col, p_left_column_title, wxLIST_FORMAT_LEFT);
	InsertColumn(s_balance_col, "Balance", wxLIST_FORMAT_RIGHT);
	if (m_show_daily_budget)
	{
		InsertColumn(s_budget_col, "Daily budget", wxLIST_FORMAT_RIGHT);
	}

	AccountReader::size_type i = 0;

	for
	(	AccountReader::const_iterator it = p_reader.begin(),
			end = p_reader.end();
		it != end;
		++it, ++i
	)
	{
		// Insert item, with string for Column 0
		InsertItem(i, bstring_to_wx(it->name()));
	
		// The item may change position due to e.g. sorting, so store the
		// original index in the item's data
		SetItemData(i, i);

		// Insert the balance string
		SetItem
		(	i,
			s_balance_col,
			finformat_wx(it->friendly_balance(), locale())
		);

		if (m_show_daily_budget)
		{
			// Insert budget string
			SetItem(i, s_budget_col, finformat_wx(it->budget(), locale()));
		}
	}

	// Reinstate the selections we remembered
	size_t const sel_sz = selected_rows.size();
	for (size_t k = 0; k != sel_sz; ++k)
	{
		SetItemState
		(	selected_rows[k],
			wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED
		);
	}

	// Configure column widths
	SetColumnWidth(s_name_col, wxLIST_AUTOSIZE_USEHEADER);
	SetColumnWidth(s_name_col, max(GetColumnWidth(s_name_col), 200));
	SetColumnWidth(s_balance_col, wxLIST_AUTOSIZE);
	SetColumnWidth(s_balance_col, max(GetColumnWidth(s_balance_col), 90));
	if (m_show_daily_budget)
	{
		SetColumnWidth(s_budget_col, wxLIST_AUTOSIZE);
		SetColumnWidth(s_budget_col, max(GetColumnWidth(s_budget_col), 90));
	}
	Layout();

	return;
}

optional<Account>
AccountListCtrl::default_account() const
{
	optional<Account> ret;
	if (GetItemCount() != 0)
	{
		assert (GetItemCount() > 0);
		ret = Account
		(	m_database_connection,
			wx_to_bstring(GetItemText(GetTopItem()))
		);
	}
	return ret;
}



}  // namespace gui
}  // namespace phatbooks
