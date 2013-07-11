// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "top_panel.hpp"
#include "account.hpp"
#include "account_list_ctrl.hpp"
#include "draft_journal_list_ctrl.hpp"
#include "draft_journal_reader.hpp"
#include "entry_list_ctrl.hpp"
#include "frame.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "sizing.hpp"
#include "transaction_ctrl.hpp"
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <cassert>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;


#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;



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
	m_notebook(0),
	m_notebook_page_1(0),
	m_notebook_page_2(0),
	m_right_column_sizer(0),
	m_bs_account_list(0),
	m_pl_account_list(0),
	m_entry_list(0),
	m_transaction_ctrl(0),
	m_draft_journal_list(0)
{
	m_top_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_notebook = new wxNotebook
	(	this,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxNB_TOP
	);
	m_right_column_sizer = new wxBoxSizer(wxVERTICAL);
	m_top_sizer->Add
	(	m_notebook,
		wxSizerFlags(6).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST | wxEAST, standard_border())
	);
	m_notebook_page_1 = new wxPanel(m_notebook, wxID_ANY);
	m_notebook_page_2 = new wxPanel(m_notebook, wxID_ANY);
	m_notebook->AddPage(m_notebook_page_1, wxString("Balances"), true);
	m_notebook->AddPage(m_notebook_page_2, wxString("Transactions"), false);
	m_top_sizer->Add
	(	m_right_column_sizer,
		wxSizerFlags(4).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST | wxEAST, standard_border())
	);
	configure_account_lists();
	configure_entry_list();
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
}

// TODO We have no consistent convention here about what is "configure" and
// what is "update", and when each such function gets called (i.e. once, or
// every update, or...?).

// TODO The TransactionCtrl and the DraftJournalListCtrl swap places in the
// display when updated. Need to fix.

void
TopPanel::configure_account_lists()
{
	assert (m_notebook_page_1);
	m_bs_account_list = AccountListCtrl::create_balance_sheet_account_list
	(	m_notebook_page_1,
		m_database_connection
	);
	m_pl_account_list = AccountListCtrl::create_pl_account_list
	(	m_notebook_page_1,
		m_database_connection
	);
	wxBoxSizer* page_1_sizer = new wxBoxSizer(wxHORIZONTAL);
	page_1_sizer->Add
	(	m_bs_account_list,
		wxSizerFlags(2).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST, standard_border())
	);
	page_1_sizer->Add
	(	m_pl_account_list,
		wxSizerFlags(3).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST, standard_border())
	);
	m_notebook_page_1->SetSizer(page_1_sizer);
	page_1_sizer->Fit(m_notebook_page_1);
	page_1_sizer->SetSizeHints(m_notebook_page_1);
	m_notebook_page_1->Fit();
	Layout();
	return;
}

void
TopPanel::configure_entry_list()
{
	assert (m_notebook_page_2);
	assert (m_notebook_page_1);
	m_entry_list = EntryListCtrl::create_actual_ordinary_entry_list
	(	m_notebook_page_2,
		m_database_connection
	);
	wxBoxSizer* page_2_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_notebook_page_2->SetSizer(page_2_sizer);
	page_2_sizer->Add(m_entry_list, wxSizerFlags(1).Expand());
	page_2_sizer->Fit(m_notebook_page_2);
	page_2_sizer->SetSizeHints(m_notebook_page_2);
	m_notebook_page_2->Fit();
	Layout();
	return;
}

void
TopPanel::configure_transaction_ctrl()
{
	assert (m_top_sizer);
	assert (m_right_column_sizer);
	vector<Account> balance_sheet_accounts;
	selected_balance_sheet_accounts(balance_sheet_accounts);
	vector<Account> pl_accounts;
	selected_pl_accounts(pl_accounts);
	configure_transaction_ctrl(balance_sheet_accounts, pl_accounts);
	return;
}

void
TopPanel::configure_transaction_ctrl(OrdinaryJournal& p_journal)
{
	TransactionCtrl* old = 0;
	assert (m_right_column_sizer);
	if (m_transaction_ctrl)
	{
		m_right_column_sizer->Detach(m_transaction_ctrl);
		old = m_transaction_ctrl;
	}
	m_transaction_ctrl = new TransactionCtrl(this, p_journal);
	m_right_column_sizer->Insert
	(	0,
		m_transaction_ctrl,
		wxSizerFlags(6).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST | wxEAST, standard_border() * 2)
	);
	/*
	if (old)
	{
		old->Destroy();  // Results in double-free (why?) WARNING
		old = 0;
	}
	*/
	Layout();
	return;
}

void
TopPanel::configure_transaction_ctrl
(	vector<Account> p_balance_sheet_accounts,
	vector<Account> p_pl_accounts
)
{
	JEWEL_DEBUG_LOG_LOCATION;
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
		JEWEL_DEBUG_LOG_LOCATION;
		if (p_pl_accounts.empty())
		{
			optional<Account> const maybe_pl_account =
				m_pl_account_list->default_account();
			if (maybe_pl_account)
			{
				p_pl_accounts.push_back(value(maybe_pl_account));
			}
		}
		JEWEL_DEBUG_LOG_LOCATION;
	}
	JEWEL_DEBUG_LOG_LOCATION;
	TransactionCtrl* old = 0;
	JEWEL_DEBUG_LOG_LOCATION;
	assert (m_right_column_sizer);
	JEWEL_DEBUG_LOG_LOCATION;
	if (m_transaction_ctrl)
	{
		m_right_column_sizer->Detach(m_transaction_ctrl);
		old = m_transaction_ctrl;
	}
	JEWEL_DEBUG_LOG_LOCATION;
	m_transaction_ctrl = new TransactionCtrl
	(	this,
		p_balance_sheet_accounts,
		p_pl_accounts,
		m_database_connection
	);
	JEWEL_DEBUG_LOG_LOCATION;
	m_right_column_sizer->Insert
	(	0,
		m_transaction_ctrl,
		wxSizerFlags(6).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST | wxEAST, standard_border() * 2)
	);
	JEWEL_DEBUG_LOG_LOCATION;
	/*
	if (old)
	{
		JEWEL_DEBUG_LOG_LOCATION;
		old->Destroy();  // Results in double-free (why?) // WARNING
		JEWEL_DEBUG_LOG_LOCATION;
		old = 0;
		JEWEL_DEBUG_LOG_LOCATION;
	}
	*/
	JEWEL_DEBUG_LOG_LOCATION;
	Layout();
	JEWEL_DEBUG_LOG_LOCATION;
	return;
}

void
TopPanel::configure_draft_journal_list_ctrl()
{
	DraftJournalListCtrl* old = 0;
	assert (m_right_column_sizer);
	if (m_draft_journal_list)
	{
		m_right_column_sizer->Detach(m_draft_journal_list);
		old = m_draft_journal_list;
	}
	UserDraftJournalReader const reader(m_database_connection);
	m_draft_journal_list = new DraftJournalListCtrl
	(	this,
		wxDefaultSize,
		reader
	);
	m_right_column_sizer->Add(m_draft_journal_list, wxSizerFlags(1).Expand());
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
TopPanel::selected_ordinary_journals(std::vector<OrdinaryJournal>& out) const
{
	vector<Entry> entries;
	m_entry_list->selected_entries(entries);
	vector<Entry>::const_iterator it = entries.begin();
	vector<Entry>::const_iterator const end = entries.end();
	for ( ; it != end; ++it)
	{
		out.push_back(it->journal<OrdinaryJournal>());
	}
	return;
}

void
TopPanel::update_for(OrdinaryJournal const& p_saved_object)
{
	m_bs_account_list->update(true);
	m_pl_account_list->update(false);
	m_entry_list->update_for_posted_journal(p_saved_object);
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for(DraftJournal const& p_saved_object)
{
	(void)p_saved_object;  // Silence compiler re. unused parameter.
	// m_bs_account_list->update(true);  // No point doing this here.
	// m_pl_account_list->update(false); // No point doing this here.
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for(Account const& p_saved_object)
{
	// TODO HIGH PRIORITY Need to update for opening balance journal as well??
	(void)p_saved_object;  // Silence compiler re. unused parameter.
	m_bs_account_list->update(true);
	m_pl_account_list->update(false);
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

	
	




}  // namespace gui
}  // namespace phatbooks

