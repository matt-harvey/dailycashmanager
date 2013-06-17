// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "top_panel.hpp"
#include "account.hpp"
#include "account_list_ctrl.hpp"
#include "frame.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_ctrl.hpp"
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/wx.h>
#include <cassert>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;

namespace phatbooks
{
namespace gui
{


TopPanel::TopPanel
(	Frame* p_parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection),
	m_top_sizer(0),
	m_bs_account_list(0),
	m_pl_account_list(0),
	m_transaction_ctrl(0)
{
	m_top_sizer = new wxBoxSizer(wxHORIZONTAL);
	configure_account_lists();
	configure_transaction_ctrl();
	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
}

void
TopPanel::configure_account_lists()
{
	assert (m_top_sizer);
	m_bs_account_list = AccountListCtrl::create_balance_sheet_account_list
	(	this,
		m_database_connection
	);
	m_pl_account_list = AccountListCtrl::create_pl_account_list
	(	this,
		m_database_connection
	);
	m_top_sizer->Add
	(	m_bs_account_list,
		wxSizerFlags(2).Expand().Border(wxNORTH | wxSOUTH | wxWEST, 15)
	);
	m_top_sizer->Add
	(	m_pl_account_list,
		wxSizerFlags(3).Expand().Border(wxNORTH | wxSOUTH | wxWEST, 15)
	);
	return;
}

void
TopPanel::configure_transaction_ctrl()
{
	assert (m_top_sizer);
	vector<Account> balance_sheet_accounts;
	selected_balance_sheet_accounts(balance_sheet_accounts);
	vector<Account> pl_accounts;
	selected_pl_accounts(pl_accounts);
	configure_transaction_ctrl(balance_sheet_accounts, pl_accounts);
	return;
}

void
TopPanel::configure_transaction_ctrl
(	vector<Account> p_balance_sheet_accounts,
	vector<Account> p_pl_accounts
)
{
	if (p_balance_sheet_accounts.size() + p_pl_accounts.size() < unsigned(2))
	{
		if (p_balance_sheet_accounts.empty())
		{
			optional<Account> const maybe_bs_account =
				m_bs_account_list->default_account();
			if (maybe_bs_account)
			{
				p_balance_sheet_accounts.push_back(value(maybe_bs_account));
			}
		}
		if (p_pl_accounts.empty())
		{
			optional<Account> const maybe_pl_account =
				m_pl_account_list->default_account();
			if (maybe_pl_account)
			{
				p_pl_accounts.push_back(value(maybe_pl_account));
			}
		}
	}
	TransactionCtrl* old = 0;
	if (m_top_sizer && m_transaction_ctrl)
	{
		m_top_sizer->Detach(m_transaction_ctrl);
		old = m_transaction_ctrl;
	}
	m_transaction_ctrl = new TransactionCtrl
	(	this,
		p_balance_sheet_accounts,
		p_pl_accounts,
		m_database_connection
	);
	m_top_sizer->Add
	(	m_transaction_ctrl,
		wxSizerFlags(4).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST | wxEAST, 15)
	);
	if (old)
	{
		old->Destroy();
		old = 0;
	}
	Layout();
	return;
}

void
TopPanel::selected_balance_sheet_accounts(vector<Account>& out) const
{
	m_bs_account_list->selected_accounts(out);
	return;
}

void
TopPanel::selected_pl_accounts(vector<Account>& out) const
{
	m_pl_account_list->selected_accounts(out);
	return;
}

void
TopPanel::update()
{
	m_bs_account_list->update(true);
	m_pl_account_list->update(false);
	configure_transaction_ctrl();
	return;
}

	
	




}  // namespace gui
}  // namespace phatbooks

