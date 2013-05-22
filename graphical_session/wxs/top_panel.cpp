// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "top_panel.hpp"
#include "account.hpp"
#include "account_list_ctrl.hpp"
#include "entry_list_ctrl.hpp"
#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/wx.h>
#include <vector>

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
	m_entry_list(0)
{
	m_top_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_bs_account_list = AccountListCtrl::create_balance_sheet_account_list
	(	this,
		m_database_connection
	);
	m_pl_account_list = AccountListCtrl::create_pl_account_list
	(	this,
		m_database_connection
	);
	m_entry_list = EntryListCtrl::create_actual_ordinary_entry_list
	(	this,
		m_database_connection
	);
	m_top_sizer->Add(m_bs_account_list, wxSizerFlags(2).Expand());
	m_top_sizer->Add(m_pl_account_list, wxSizerFlags(3).Expand());
	m_top_sizer->Add(m_entry_list, wxSizerFlags(4).Expand());
	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
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









}  // namespace gui
}  // namespace phatbooks

