/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gui/top_panel.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "draft_journal.hpp"
#include "draft_journal_table_iterator.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "proto_journal.hpp"
#include "dcm_database_connection.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include "gui/account_list_ctrl.hpp"
#include "gui/draft_journal_list_ctrl.hpp"
#include "gui/entry_list_panel.hpp"
#include "gui/frame.hpp"
#include "gui/reconciliation_list_panel.hpp"
#include "gui/report_panel.hpp"
#include "gui/sizing.hpp"
#include "gui/transaction_ctrl.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/window.h>
#include <wx/wupdlock.h>
#include <iterator>
#include <map>
#include <set>
#include <utility>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::Handle;
using std::begin;
using std::end;
using std::map;
using std::set;
using std::vector;

namespace dcm
{
namespace gui
{

TopPanel::TopPanel
(	Frame* p_parent,
	DcmDatabaseConnection& p_database_connection
):
	wxPanel
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection),
	m_top_sizer(nullptr),
	m_notebook(nullptr),
	m_notebook_page_accounts(nullptr),
	m_notebook_page_transactions(nullptr),
	m_notebook_page_reconciliations(nullptr),
	m_notebook_page_reports(nullptr),
	m_right_column_sizer(nullptr),
	m_bs_account_list(nullptr),
	m_pl_account_list(nullptr),
	m_entry_list_panel(nullptr),
	m_reconciliation_panel(nullptr),
	m_report_panel(nullptr),
	m_transaction_ctrl(nullptr),
	m_draft_journal_list(nullptr)
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

// TODO LOW PRIORITY We have no consistent convention here about what is named
// "configure" and what is named "update", in relation to when each such
// function gets called (i.e. once, or every update, or...?).

void
TopPanel::configure_account_lists()
{
	JEWEL_ASSERT (m_notebook_page_accounts);
	m_bs_account_list = new AccountListCtrl
	(	m_notebook_page_accounts,
		m_database_connection,
		AccountSuperType::balance_sheet
	);
	m_pl_account_list = new AccountListCtrl
	(	m_notebook_page_accounts,
		m_database_connection,
		AccountSuperType::pl
	);
	map<AccountSuperType, sqloxx::Id> const fav_accts =
		favourite_accounts(m_database_connection);
	JEWEL_ASSERT (fav_accts.size() == 2);
	m_bs_account_list->select_only
	(	Handle<Account>
		(	m_database_connection,
			fav_accts.at(AccountSuperType::balance_sheet)
		)
	);
	m_pl_account_list->select_only
	(	Handle<Account>
		(	m_database_connection,
			fav_accts.at(AccountSuperType::pl)
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
	JEWEL_ASSERT (m_notebook_page_transactions);
	JEWEL_ASSERT (!m_entry_list_panel);
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
	JEWEL_ASSERT (m_notebook_page_reconciliations);
	JEWEL_ASSERT (!m_reconciliation_panel);
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
	JEWEL_ASSERT (m_notebook_page_reports);
	JEWEL_ASSERT (!m_report_panel);
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

ProtoJournal
TopPanel::make_proto_journal() const
{
	vector<Handle<Account> > balance_sheet_accounts;
	vector<Handle<Account> > pl_accounts;
	selected_balance_sheet_accounts(balance_sheet_accounts);
	selected_pl_accounts(pl_accounts);
	if (balance_sheet_accounts.size() + pl_accounts.size() < unsigned(2))
	{
		if (balance_sheet_accounts.empty())
		{
			auto const bs_account = m_bs_account_list->default_account();
			if (bs_account) balance_sheet_accounts.push_back(bs_account);
		}
		if (pl_accounts.empty())
		{
			auto const pl_account = m_pl_account_list->default_account();
			if (pl_account) pl_accounts.push_back(pl_account);
		}
	}
	ProtoJournal ret;

	// bare scope
	{
		Handle<Account> account_x(m_database_connection);
		Handle<Account> account_y(m_database_connection);
		if (balance_sheet_accounts.empty())
		{
			JEWEL_ASSERT (pl_accounts.size() >= 2);
			account_x = pl_accounts[0];
			account_y = pl_accounts[1];
		}
		else if (pl_accounts.empty())
		{
			JEWEL_ASSERT (balance_sheet_accounts.size() >= 2);
			account_x = balance_sheet_accounts[0];
			account_y = balance_sheet_accounts[1];
		}
		else
		{
			JEWEL_ASSERT (!balance_sheet_accounts.empty());
			JEWEL_ASSERT (!pl_accounts.empty());
			account_x = balance_sheet_accounts[0];
			account_y = pl_accounts[0];
		}
		if (account_y->account_type() == AccountType::revenue)
		{
			using std::swap;
			swap(account_x, account_y);
		}
		JEWEL_ASSERT (account_x->has_id());
		JEWEL_ASSERT (account_y->has_id());
		TransactionType const initial_transaction_type =
			natural_transaction_type(account_x, account_y);
		assert_transaction_type_validity(initial_transaction_type);
		ret.set_transaction_type(initial_transaction_type);
		Handle<Account> const accounts[] = {account_x, account_y};
		auto const sz = static_cast<size_t>(end(accounts) - begin(accounts));
		for (size_t i = 0; i != sz; ++i)
		{
			Handle<Account> const& account = accounts[i];
			Handle<Entry> const entry(m_database_connection);
			entry->set_account(account);
			entry->set_comment(wxString());
			entry->set_transaction_side
			(	(i == 0)?
				TransactionSide::source:
				TransactionSide::destination
			);
			entry->set_amount(Decimal(0, account->commodity()->precision()));
			entry->set_whether_reconciled(false);
			ret.push_entry(entry);	
		}
	}
	return ret;
}

ProtoJournal
TopPanel::make_proto_envelope_transfer() const
{
	ProtoJournal ret;
	auto const default_pl_account = m_pl_account_list->default_account();

	// NOTE This is guaranteed because m_pl_account_list cannot be
	// empty due to restrictions on user hiding all the Accounts of any given
	// AccountType, and SetupWizard forcing user to create Accounts such that
	// each AccountListCtrl will contain at least one Account.
	JEWEL_HARD_ASSERT (default_pl_account);

	vector<Handle<Account> > const accounts
	{	default_pl_account,
		default_pl_account
	};
	for (size_t i = 0; i != accounts.size(); ++i)
	{
		Handle<Account> const account = accounts[i];
		Handle<Entry> const entry(m_database_connection);
		entry->set_account(account);
		entry->set_comment(wxString());
		entry->set_transaction_side
		(	(i == 0)?
			TransactionSide::source:
			TransactionSide::destination
		);
		entry->set_amount(Decimal(0, account->commodity()->precision()));
		entry->set_whether_reconciled(false);
		ret.push_entry(entry);
	}
	ret.set_comment(wxString());
	ret.set_transaction_type(TransactionType::envelope);
	JEWEL_ASSERT (ret.entries().size() == 2);
	return ret;
}

void
TopPanel::configure_transaction_ctrl()
{
	JEWEL_ASSERT (m_top_sizer);
	JEWEL_ASSERT (m_right_column_sizer);
	if (m_transaction_ctrl)
	{
		m_transaction_ctrl->reset();
	}
	else
	{
		wxWindowUpdateLocker const window_update_locker(this);
		ProtoJournal proto_journal = make_proto_journal();

		// NOTE This relies on our having forced the user to create at least
		// one Account of each AccountSuperType when the database is first
		// set up, and preventing the deletion of Accounts or the editing
		// of their AccountType thereafter.
		JEWEL_ASSERT (proto_journal.entries().size() >= 2);

		m_transaction_ctrl = new TransactionCtrl
		(	this,
			wxSize(GetClientSize().x, 10000),
			proto_journal,
			m_database_connection
		);
		auto const sides = wxNORTH | wxSOUTH | wxWEST | wxEAST;
		auto const bwidths = standard_border() * 2;
		auto const flags = wxSizerFlags(6).Expand().Border(sides, bwidths);
		m_right_column_sizer->Insert(0, m_transaction_ctrl, flags);
		Layout();
	}
	return;
}

void
TopPanel::configure_draft_journal_list_ctrl()
{
	JEWEL_LOG_TRACE();
	DraftJournalListCtrl* old = nullptr;
	JEWEL_ASSERT (m_right_column_sizer);
	if (m_draft_journal_list)
	{
		m_right_column_sizer->Detach(m_draft_journal_list);
		old = m_draft_journal_list;
	}
	m_draft_journal_list = new DraftJournalListCtrl
	(	this,
		wxDefaultSize,
		make_name_ordered_user_draft_journal_table_iterator
		(	m_database_connection
		),
		(DraftJournalTableIterator()),
		m_database_connection
	);
	m_right_column_sizer->Add(m_draft_journal_list, wxSizerFlags(1).Expand());
	if (old)
	{
		JEWEL_LOG_TRACE();
		old->Destroy();
		old = nullptr;
	}
	Layout();
	JEWEL_LOG_TRACE();
	return;
}

bool
TopPanel::toggle_show_hidden_accounts
(	AccountSuperType p_account_super_type
)
{
	switch (p_account_super_type)
	{
	case AccountSuperType::balance_sheet:
		return m_bs_account_list->toggle_showing_hidden();
	case AccountSuperType::pl:
		return m_pl_account_list->toggle_showing_hidden();
	default:
		JEWEL_HARD_ASSERT (false);
	}
	JEWEL_HARD_ASSERT (false);
}

void
TopPanel::selected_balance_sheet_accounts(vector<Handle<Account> >& out) const
{
	set<sqloxx::Id> selected_ids;
	m_bs_account_list->selected_accounts(selected_ids);
	for (sqloxx::Id const selected_id: selected_ids)
	{
		out.push_back(Handle<Account>(m_database_connection, selected_id));
	}
	return;
}

void
TopPanel::selected_pl_accounts(vector<Handle<Account> >& out) const
{
	set<sqloxx::Id> selected_ids;
	m_pl_account_list->selected_accounts(selected_ids);
	for (sqloxx::Id const selected_id: selected_ids)
	{
		out.push_back(Handle<Account>(m_database_connection, selected_id));
	}
	return;
}

void
TopPanel::selected_ordinary_journals
(	vector<Handle<OrdinaryJournal> >& out
) const
{
	vector<Handle<Entry> > entries;
	JEWEL_ASSERT (m_notebook);
	wxWindow* const page = m_notebook->GetCurrentPage();
	if (page == static_cast<wxWindow*>(m_notebook_page_transactions))
	{
		entries = m_entry_list_panel->selected_entries();
	}
	else if (page == static_cast<wxWindow*>(m_notebook_page_reconciliations))
	{
		entries = m_reconciliation_panel->selected_entries();
	}
	for (Handle<Entry> const& entry: entries)
	{
		Handle<OrdinaryJournal> const oj
		(	entry->database_connection(),
			entry->journal_id()
		);
		out.push_back(oj);
	}
	return;
}

void
TopPanel::selected_draft_journals(vector<Handle<DraftJournal> >& out) const
{
	m_draft_journal_list->selected_draft_journals(out);
	return;
}

void
TopPanel::update_for_new(Handle<OrdinaryJournal> const& p_saved_object)
{
	m_bs_account_list->update();
	m_pl_account_list->update();
	m_entry_list_panel->update_for_new(p_saved_object);
	m_reconciliation_panel->update_for_new(p_saved_object);
	m_report_panel->update_for_new(p_saved_object);
	// configure_transaction_ctrl();  // Don't do this!
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_new(Handle<DraftJournal> const& p_saved_object)
{
	(void)p_saved_object;  // Silence compiler re. unused parameter.
	// m_bs_account_list->update();  // No point doing this here.
	// m_pl_account_list->update(); // No point doing this here.
	// configure_transaction_ctrl();  // Don't do this!
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_new(Handle<Account> const& p_saved_object)
{
	m_bs_account_list->update();
	m_pl_account_list->update();
	m_entry_list_panel->update_for_new(p_saved_object);
	m_reconciliation_panel->update_for_new(p_saved_object);
	m_report_panel->update_for_new(p_saved_object);
	m_transaction_ctrl->update_for_new(p_saved_object);
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_amended(Handle<OrdinaryJournal> const& p_saved_object)
{
	m_bs_account_list->update();
	m_pl_account_list->update();
	m_entry_list_panel->update_for_amended(p_saved_object);
	m_reconciliation_panel->update_for_amended(p_saved_object);
	m_report_panel->update_for_amended(p_saved_object);
	// configure_transaction_ctrl();  // Don't do this!
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_amended(Handle<DraftJournal> const& p_saved_object)
{
	(void)p_saved_object;  // Silence compiler re. unused parameter.
	m_bs_account_list->update();
	m_pl_account_list->update();
	// m_entry_list_panel->update_for_amended(p_saved_object);  // Does not apply for DraftJournal.
	// m_reconciliation_panel->update_for_amended(p_saved_object);  // Does not apply for DraftJournal.
	// configure_transaction_ctrl();  // Don't do this!
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_amended(Handle<Account> const& p_saved_object)
{
	m_bs_account_list->update();
	m_pl_account_list->update();
	m_entry_list_panel->update_for_amended(p_saved_object);
	m_reconciliation_panel->update_for_amended(p_saved_object);
	m_report_panel->update_for_amended(p_saved_object);
	m_transaction_ctrl->update_for_amended(p_saved_object);
	configure_draft_journal_list_ctrl();
	return;
}
	
void
TopPanel::update_for_amended_budget(Handle<Account> const& p_account)
{
	(void)p_account;  // Silence compiler re. unused parameter.
	JEWEL_ASSERT (p_account->account_super_type() == AccountSuperType::pl);
	JEWEL_ASSERT (m_pl_account_list);
	JEWEL_ASSERT (m_report_panel);
	m_pl_account_list->update();
	m_report_panel->update_for_amended_budget(p_account);
	return;
}

void
TopPanel::update_for_reconciliation_status(Handle<Entry> const& p_entry)
{
	JEWEL_ASSERT (m_transaction_ctrl);
	m_transaction_ctrl->update_for_reconciliation_status(p_entry);
	return;
}

void
TopPanel::update_for_deleted_ordinary_journal(sqloxx::Id p_doomed_id)
{
	(void)p_doomed_id;  // Silence compiler re. unused parameter.
	m_bs_account_list->update();
	m_pl_account_list->update();
	// configure_transaction_ctrl();  // Don't do this!
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_deleted_draft_journal(sqloxx::Id p_doomed_id)
{
	JEWEL_LOG_TRACE();
	(void)p_doomed_id;  // Silence compiler re. unused parameter.
	// configure_transaction_ctrl();  // Don't do this!
	configure_draft_journal_list_ctrl();
	return;
}

void
TopPanel::update_for_deleted_ordinary_entries
(	vector<sqloxx::Id> const& p_doomed_ids
)
{
	m_entry_list_panel->update_for_deleted(p_doomed_ids);
	m_reconciliation_panel->update_for_deleted(p_doomed_ids);
	m_report_panel->update_for_deleted(p_doomed_ids);
	return;
}

void
TopPanel::update_for_deleted_draft_entries
(	vector<sqloxx::Id> const& p_doomed_ids
)
{
	JEWEL_LOG_TRACE();
	(void)p_doomed_ids;  // Silence compiler re. unused parameter

	// Nothing to do, as DraftJournal Entries are not displayed individually
	// in the top panel (except possibly TransactionCtrl, but that can take
	// care of itself). The deleted DraftJournal itself is processed via
	// update_for_deleted_draft_journal.
	JEWEL_LOG_TRACE();
	return;
}

}  // namespace gui
}  // namespace dcm

