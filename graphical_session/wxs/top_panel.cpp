// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "top_panel.hpp"
#include "account.hpp"
#include "account_list_ctrl.hpp"
#include "account_type.hpp"
#include "draft_journal_list_ctrl.hpp"
#include "draft_journal_reader.hpp"
#include "entry.hpp"
#include "entry_list_panel.hpp"
#include "frame.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include "reconciliation_list_panel.hpp"
#include "report_panel.hpp"
#include "sizing.hpp"
#include "transaction_ctrl.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <map>
#include <boost/optional.hpp>
#include <jewel/array_utilities.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/window.h>
#include <wx/wupdlock.h>
#include <cassert>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::num_elements;
using jewel::value;
using std::map;
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
	m_notebook_page_accounts(0),
	m_notebook_page_transactions(0),
	m_notebook_page_reconciliations(0),
	m_notebook_page_reports(0),
	m_right_column_sizer(0),
	m_bs_account_list(0),
	m_pl_account_list(0),
	m_entry_list_panel(0),
	m_reconciliation_panel(0),
	m_report_panel(0),
	m_transaction_ctrl(0),
	m_draft_journal_list(0)
{
	m_top_sizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(m_top_sizer);
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
	m_notebook_page_accounts = new wxPanel(m_notebook, wxID_ANY);
	m_notebook_page_transactions = new wxPanel(m_notebook, wxID_ANY);
	m_notebook_page_reconciliations = new wxPanel(m_notebook, wxID_ANY);
	m_notebook_page_reports = new wxPanel(m_notebook, wxID_ANY);
	m_notebook->AddPage
	(	m_notebook_page_accounts,
		wxString("Balances"),
		true
	);
	m_notebook->AddPage
	(	m_notebook_page_transactions,
		wxString("Transactions"),
		false
	);
	m_notebook->AddPage
	(	m_notebook_page_reconciliations,
		wxString("Reconciliations"),
		false
	);
	m_notebook->AddPage
	(	m_notebook_page_reports,
		wxString("Reports"),
		false
	);
	m_top_sizer->Add
	(	m_right_column_sizer,
		wxSizerFlags(4).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST | wxEAST, standard_border())
	);
	configure_account_lists();
	configure_entry_list();
	configure_reconciliation_page();
	configure_report_page();
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
}

// TODO We have no consistent convention here about what is "configure" and
// what is "update", and when each such function gets called (i.e. once, or
// every update, or...?).

void
TopPanel::configure_account_lists()
{
	assert (m_notebook_page_accounts);
	m_bs_account_list = AccountListCtrl::create_balance_sheet_account_list
	(	m_notebook_page_accounts,
		m_database_connection
	);
	m_pl_account_list = AccountListCtrl::create_pl_account_list
	(	m_notebook_page_accounts,
		m_database_connection
	);
	map<account_super_type::AccountSuperType, Account::Id> const fav_accts =
		favourite_accounts(m_database_connection);
	assert (fav_accts.size() == 2);
	m_bs_account_list->select_only
	(	Account
		(	m_database_connection,
			fav_accts.at(account_super_type::balance_sheet)
		)
	);
	m_pl_account_list->select_only
	(	Account
		(	m_database_connection,
			fav_accts.at(account_super_type::pl)
		)
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
	m_notebook_page_accounts->SetSizer(page_1_sizer);
	page_1_sizer->Fit(m_notebook_page_accounts);
	page_1_sizer->SetSizeHints(m_notebook_page_accounts);
	m_notebook_page_accounts->Fit();
	Layout();
	return;
}

void
TopPanel::configure_entry_list()
{
	assert (m_notebook_page_transactions);
	assert (!m_entry_list_panel);
	m_entry_list_panel = new EntryListPanel
	(	m_notebook_page_transactions,
		m_database_connection
	);
	wxBoxSizer* page_2_sizer = new wxBoxSizer(wxHORIZONTAL);
	page_2_sizer->Add(m_entry_list_panel, wxSizerFlags(1).Expand());
	m_notebook_page_transactions->SetSizer(page_2_sizer);
	page_2_sizer->Fit(m_notebook_page_transactions);
	page_2_sizer->SetSizeHints(m_notebook_page_transactions);
	m_notebook_page_transactions->Fit();
	Layout();
	return;
}

void
TopPanel::configure_reconciliation_page()
{
	assert (m_notebook_page_reconciliations);
	assert (!m_reconciliation_panel);
	m_reconciliation_panel = new ReconciliationListPanel
	(	m_notebook_page_reconciliations,
		m_database_connection,
		true
	);
	wxBoxSizer* page_3_sizer = new wxBoxSizer(wxHORIZONTAL);
	page_3_sizer->Add(m_reconciliation_panel, wxSizerFlags(1).Expand());
	m_notebook_page_reconciliations->SetSizer(page_3_sizer);
	page_3_sizer->Fit(m_notebook_page_reconciliations);
	page_3_sizer->SetSizeHints(m_notebook_page_reconciliations);
	m_notebook_page_reconciliations->Fit();
	Layout();
	return;
}

void
TopPanel::configure_report_page()
{
	assert (m_notebook_page_reports);
	assert (!m_report_panel);
	m_report_panel =
		new ReportPanel(m_notebook_page_reports, m_database_connection);
	wxBoxSizer* page_4_sizer = new wxBoxSizer(wxHORIZONTAL);
	page_4_sizer->Add(m_report_panel, wxSizerFlags(10).Expand());

	m_notebook_page_reports->SetSizer(page_4_sizer);
	page_4_sizer->Fit(m_notebook_page_reports);
	page_4_sizer->SetSizeHints(m_notebook_page_reports);
	m_notebook_page_reports->Fit();
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
TopPanel::configure_transaction_ctrl
(	vector<Account> p_balance_sheet_accounts,
	vector<Account> p_pl_accounts
)
{
	wxWindowUpdateLocker window_update_locker(this);
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
	assert (m_right_column_sizer);
	if (m_transaction_ctrl)
	{
		m_right_column_sizer->Detach(m_transaction_ctrl);
		old = m_transaction_ctrl;
	}

	ProtoJournal proto_journal;

	// TODO Move this next bit into a separate function
	// bare scope
	{
		Account account_x(m_database_connection);
		Account account_y(m_database_connection);
		if (p_balance_sheet_accounts.empty())
		{
			assert (p_pl_accounts.size() >= 2);
			account_x = p_pl_accounts[0];
			account_y = p_pl_accounts[1];
		}
		else if (p_pl_accounts.empty())
		{
			assert (p_balance_sheet_accounts.size() >= 2);
			account_x = p_balance_sheet_accounts[0];
			account_y = p_balance_sheet_accounts[1];
		}
		else
		{
			assert (!p_balance_sheet_accounts.empty());
			assert (!p_pl_accounts.empty());
			account_x = p_balance_sheet_accounts[0];
			account_y = p_pl_accounts[0];
		}
		if (account_y.account_type() == account_type::revenue)
		{
			using std::swap;
			swap(account_x, account_y);
		}
		assert (account_x.has_id());
		assert (account_y.has_id());
		transaction_type::TransactionType const initial_transaction_type =
			natural_transaction_type(account_x, account_y);
		assert_transaction_type_validity(initial_transaction_type);
		proto_journal.set_transaction_type(initial_transaction_type);
		Account const accounts[] = {account_x, account_y};
		for (size_t i = 0; i != num_elements(accounts); ++i)
		{
			Account const& account = accounts[i];
			Entry entry(m_database_connection);
			entry.set_account(account);
			entry.set_comment(BString());
			entry.set_transaction_side
			(	(i == 0)?
				transaction_side::source:
				transaction_side::destination
			);
			entry.set_amount(Decimal(0, account.commodity().precision()));
			entry.set_whether_reconciled(false);
			proto_journal.push_entry(entry);	
		}
	}

	m_transaction_ctrl = new TransactionCtrl
	(	this,
		wxSize(GetClientSize().x, 10000),
		proto_journal,
		m_database_connection
	);
	m_right_column_sizer->Insert
	(	0,
		m_transaction_ctrl,
		wxSizerFlags(6).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST | wxEAST, standard_border() * 2)
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
		reader,
		m_database_connection
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

bool
TopPanel::toggle_show_hidden_accounts
(	account_super_type::AccountSuperType p_account_super_type
)
{
	switch (p_account_super_type)
	{
	case account_super_type::balance_sheet:
		return m_bs_account_list->toggle_showing_hidden();
	case account_super_type::pl:
		return m_pl_account_list->toggle_showing_hidden();
	default:
		assert (false);
	}
	assert (false);
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
TopPanel::selected_ordinary_journals(vector<OrdinaryJournal>& out) const
{
	vector<Entry> entries;
	assert (m_notebook);
	wxWindow* const page = m_notebook->GetCurrentPage();
	if (page == static_cast<wxWindow*>(m_notebook_page_transactions))
	{
		m_entry_list_panel->selected_entries(entries);
	}
	else if (page == static_cast<wxWindow*>(m_notebook_page_reconciliations))
	{
		m_reconciliation_panel->selected_entries(entries);
	}
	vector<Entry>::const_iterator it = entries.begin();
	vector<Entry>::const_iterator const end = entries.end();
	for ( ; it != end; ++it)
	{
		out.push_back(it->journal<OrdinaryJournal>());
	}
	return;
}

void
TopPanel::selected_draft_journals(vector<DraftJournal>& out) const
{
	m_draft_journal_list->selected_draft_journals(out);
	return;
}

void
TopPanel::update_for_new(OrdinaryJournal const& p_saved_object)
{
	m_bs_account_list->update();
	m_pl_account_list->update();
	m_entry_list_panel->update_for_new(p_saved_object);
	m_reconciliation_panel->update_for_new(p_saved_object);
	m_report_panel->update_for_new(p_saved_object);
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_new(DraftJournal const& p_saved_object)
{
	(void)p_saved_object;  // Silence compiler re. unused parameter.
	// m_bs_account_list->update();  // No point doing this here.
	// m_pl_account_list->update(); // No point doing this here.
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_new(Account const& p_saved_object)
{
	m_bs_account_list->update();
	m_pl_account_list->update();
	m_entry_list_panel->update_for_new(p_saved_object);
	m_reconciliation_panel->update_for_new(p_saved_object);
	m_report_panel->update_for_new(p_saved_object);
	// TODO This will kill the existing contents of the TransactionCtrl.
	// Do we really want to do this?
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_amended(OrdinaryJournal const& p_saved_object)
{
	m_bs_account_list->update();
	m_pl_account_list->update();
	m_entry_list_panel->update_for_amended(p_saved_object);
	m_reconciliation_panel->update_for_amended(p_saved_object);
	m_report_panel->update_for_new(p_saved_object);
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_amended(DraftJournal const& p_saved_object)
{
	(void)p_saved_object;  // Silence compiler re. unused parameter.
	m_bs_account_list->update();
	m_pl_account_list->update();
	// m_entry_list_panel->update_for_amended(p_saved_object);  // Does not apply for DraftJournal.
	// m_reconciliation_panel->update_for_amended(p_saved_object);  // Does not apply for
	// DraftJournal.
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_amended(Account const& p_saved_object)
{
	m_bs_account_list->update();
	m_pl_account_list->update();
	m_entry_list_panel->update_for_amended(p_saved_object);
	m_reconciliation_panel->update_for_amended(p_saved_object);
	m_report_panel->update_for_new(p_saved_object);

	// TODO This will kill the existing contents of the TransactionCtrl.
	// Do we really want to do this?
	
	// TODO We should still make sure that if the Account has changed
	// name or visibility, we CAREFULLY update the AccountCtrls within
	// TransactionCtrl accordingly - noting that we don't want to cause
	// an Account to disappear from an AccountCtrl if it is the one
	// that is currently selected.
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}
	
void
TopPanel::update_for_amended_budget(Account const& p_account)
{
	(void)p_account;  // Silence compiler re. unused parameter.
	assert (super_type(p_account.account_type()) == account_super_type::pl);
	m_pl_account_list->update();
	// TODO Do we need to update ReportPanel for amended budget?
	return;
}

void
TopPanel::update_for_reconciliation_status(Entry const& p_entry)
{
	assert (m_transaction_ctrl);
	m_transaction_ctrl->update_for_reconciliation_status(p_entry);
	return;
}

void
TopPanel::update_for_deleted_ordinary_journal(OrdinaryJournal::Id p_doomed_id)
{
	(void)p_doomed_id;  // Silence compiler re. unused parameter.
	m_bs_account_list->update();
	m_pl_account_list->update();
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_deleted_draft_journal(DraftJournal::Id p_doomed_id)
{
	(void)p_doomed_id;  // Silence compiler re. unused parameter.
	configure_transaction_ctrl();
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_deleted_ordinary_entries
(	vector<Entry::Id> const& p_doomed_ids
)
{
	m_entry_list_panel->update_for_deleted(p_doomed_ids);
	m_reconciliation_panel->update_for_deleted(p_doomed_ids);
	m_report_panel->update_for_deleted(p_doomed_ids);
	return;
}

void
TopPanel::update_for_deleted_draft_entries
(	vector<Entry::Id> const& p_doomed_ids
)
{
	(void)p_doomed_ids;  // Silence compiler re. unused parameter

	// Nothing to do, as DraftJournal Entries are not displayed individually
	// in the top panel (except possibly TransactionCtrl, but that can take
	// care of itself). The deleted DraftJournal itself is processed via
	// update_for_deleted_draft_journal.
	return;
}

}  // namespace gui
}  // namespace phatbooks

