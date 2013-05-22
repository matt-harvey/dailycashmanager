// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_list_ctrl.hpp"
#include "app.hpp"
#include "account.hpp"
#include "account_reader.hpp"
#include "b_string.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/wx.h>
#include <cassert>
#include <vector>

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
	AccountListCtrl* ret = new AccountListCtrl(parent, reader, dbc, false);
	return ret;
}

AccountListCtrl*
AccountListCtrl::create_pl_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	PLAccountReader const reader(dbc);
	AccountListCtrl* ret = new AccountListCtrl(parent, reader, dbc, true);
	return ret;
}

AccountListCtrl::AccountListCtrl
(	wxWindow* p_parent,
	AccountReaderBase const& p_reader,
	PhatbooksDatabaseConnection& p_database_connection,
	bool p_show_daily_budget
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
	m_database_connection(p_database_connection)
{
	InsertColumn(s_name_col, "Name", wxLIST_FORMAT_LEFT);
	InsertColumn(s_balance_col, "Balance", wxLIST_FORMAT_RIGHT);
	if (p_show_daily_budget)
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

		if (p_show_daily_budget)
		{
			// Insert budget string
			SetItem(i, s_budget_col, finformat_wx(it->budget(), locale()));
		}
	}
	SetColumnWidth(s_name_col, wxLIST_AUTOSIZE);
	SetColumnWidth(s_balance_col, wxLIST_AUTOSIZE);
	if (p_show_daily_budget)
	{
		SetColumnWidth(s_budget_col, wxLIST_AUTOSIZE);
	}
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

}  // namespace gui
}  // namespace phatbooks
