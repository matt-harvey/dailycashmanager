// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "reconciliation_entry_list_ctrl.hpp"
#include "entry.hpp"
#include "filtered_entry_list_ctrl.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "ordinary_journal.hpp"
#include "persistent_object_event.hpp"
#include "reconciliation_status_marker.hpp"
#include "summary_datum.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/listctrl.h>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using std::vector;

// For debugging
	#include <jewel/log.hpp>
	#include <iostream>
	using std::endl;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

// TODO We can use "EVT_LIST_ITEM_COL_CLICK" to activate this
// on left click of the "Reconciled" column - which is more intuitive.
BEGIN_EVENT_TABLE(ReconciliationEntryListCtrl, FilteredEntryListCtrl)
	EVT_LIST_ITEM_RIGHT_CLICK
	(	wxID_ANY,
		ReconciliationEntryListCtrl::on_item_right_click
	)
END_EVENT_TABLE()

namespace
{
	int comment_col_num()
	{
		return 1;
	}
	int amount_col_num()
	{
		return 2;
	}
	int reconciled_col_num()
	{
		return 3;
	}
	int anon_num_columns()
	{
		return 4;
	}

}  // end anonymous namespace

ReconciliationEntryListCtrl::ReconciliationEntryListCtrl
(	ReconciliationListPanel* p_parent,
	wxSize const& p_size,
	Account const& p_account,
	gregorian::date const& p_min_date,
	gregorian::date const& p_max_date
):
	FilteredEntryListCtrl
	(	p_parent,
		p_size,
		p_account,
		optional<gregorian::date>(p_min_date),
		optional<gregorian::date>(p_max_date)
	),
	m_max_date(p_max_date),
	m_summary_data(0),
	m_closing_balance(0, p_account.commodity().precision()),
	m_reconciled_closing_balance(0, p_account.commodity().precision())
{
}

ReconciliationEntryListCtrl::~ReconciliationEntryListCtrl()
{
	delete m_summary_data;
	m_summary_data = 0;
}

void
ReconciliationEntryListCtrl::do_set_non_date_columns
(	long p_row,
	Entry const& p_entry
)
{
	SetItem(p_row, comment_col_num(), p_entry.comment());
	SetItem
	(	p_row,
		amount_col_num(),
		finformat_wx
		(	p_entry.amount(),
			locale(),
			DecimalFormatFlags().clear(string_flags::dash_for_zero)
		)
	);
	SetItem
	(	p_row,
		reconciled_col_num(),
		reconciliation_status_marker(p_entry.is_reconciled())
	);
	assert (num_columns() == 4);
	return;
}

void
ReconciliationEntryListCtrl::do_insert_non_date_columns()
{
	InsertColumn
	(	comment_col_num(),
		wxString("Memo"),
		wxLIST_FORMAT_LEFT
	);
	InsertColumn
	(	amount_col_num(),
		wxString("Amount"),
		wxLIST_FORMAT_RIGHT
	);
	InsertColumn
	(	reconciled_col_num(),
		wxString("Reconciled?"),
		wxLIST_FORMAT_RIGHT
	);
	assert (num_columns() == 4);
	return;
}

bool
ReconciliationEntryListCtrl::do_approve_entry(Entry const& p_entry) const
{
	if (p_entry.account() != account())
	{
		return false;
	}
	gregorian::date const date = p_entry.date();
	if (date > max_date())
	{
		return false;
	}
	if (date < min_date())
	{
		// We include unreconciled Entries even if they're prior to the
		// min_date(), providing they're not later than max_date().
		assert (date <= max_date());
		assert
		(	(date > database_connection().opening_balance_journal_date()) ||
			p_entry.is_reconciled()
		);
		return !p_entry.is_reconciled();
	}
	assert (p_entry.account() == account());
	assert (date >= min_date());
	assert (date <= max_date());
	return true;
}

int
ReconciliationEntryListCtrl::do_get_comment_col_num() const
{
	return comment_col_num();
}

int
ReconciliationEntryListCtrl::do_get_num_columns() const
{
	return anon_num_columns();
}

vector<SummaryDatum> const&
ReconciliationEntryListCtrl::do_get_summary_data() const
{
	assert (!m_summary_data->empty());
	m_summary_data->at(0).set_amount(m_closing_balance);
	m_summary_data->at(1).set_amount(m_reconciled_closing_balance);
	return *m_summary_data;
}

void
ReconciliationEntryListCtrl::do_initialize_summary_data()
{
	m_closing_balance = Decimal(0, account().commodity().precision());
	m_reconciled_closing_balance =
		Decimal(0, account().commodity().precision());
	assert (!m_summary_data);
	m_summary_data = new std::vector<SummaryDatum>;
	SummaryDatum a
	(	wxString("Closing balance"),
		m_closing_balance
	);
	m_summary_data->push_back(a);
	SummaryDatum b
	(	wxString("Reconciled balance"),
		m_reconciled_closing_balance
	);
	m_summary_data->push_back(b);
	return;
}

void
ReconciliationEntryListCtrl::do_process_candidate_entry_for_summary
(	Entry const& p_entry
)
{
	if (p_entry.account() != account())
	{
		return;
	}
	assert (p_entry.account() == account());
	jewel::Decimal const amount = p_entry.amount();
	if (p_entry.date() <= max_date())
	{
		m_closing_balance += amount;
		if (p_entry.is_reconciled()) m_reconciled_closing_balance += amount;
	}
	return;
}

void
ReconciliationEntryListCtrl::do_process_removal_for_summary(long p_row)
{
	// Item is in the visible list so must be removed from m_closing_balance
	Decimal const amount = amount_for_row(p_row);
	m_closing_balance -= amount;

	// Check whether the item is marked as reconciled in the visible list.
	// If it is, then its removal should impact m_reconciled_closing_balance.
	wxListItem item;
	item.SetId(p_row);
	item.SetColumn(reconciled_col_num());
	GetItem(item);
	if (item.GetText() == reconciliation_status_marker(true))
	{
		m_reconciled_closing_balance -= amount;	
	}
	return;	
}

void
ReconciliationEntryListCtrl::on_item_right_click(wxListEvent& event)
{
	int const col = reconciled_col_num();
	Entry::Id const entry_id = event.GetData();	
	long const pos = event.GetIndex();
	assert (FindItem(-1, entry_id) == pos);
	assert (entry_id >= 0);
	assert (GetItemData(pos) == static_cast<size_t>(entry_id));

	Entry entry(database_connection(), entry_id);
	bool const old_reconciliation_status = entry.is_reconciled();
	entry.set_whether_reconciled(!old_reconciliation_status);
	SetItem(pos, col, reconciliation_status_marker(entry.is_reconciled()));
	if (entry.is_reconciled())
	{
		m_reconciled_closing_balance += entry.amount();
	}
	else
	{
		m_reconciled_closing_balance -= entry.amount();
	}
	entry.save();

	assert (entry.has_id());
	assert (entry.id() == entry_id);
	PersistentObjectEvent::fire
	(	this,
		PHATBOOKS_RECONCILIATION_STATUS_EVENT,
		entry_id
	);

	ReconciliationListPanel* parent =
		dynamic_cast<ReconciliationListPanel*>(GetParent());
	assert (parent);
	parent->postconfigure_summary();
	return;
}

Decimal
ReconciliationEntryListCtrl::amount_for_row(long p_row) const
{
	wxListItem item;
	item.SetId(p_row);
	item.SetColumn(amount_col_num());
	GetItem(item);
	Decimal ret = wx_to_decimal(item.GetText(), locale());
	return ret;
}

boost::gregorian::date
ReconciliationEntryListCtrl::max_date() const
{
	return m_max_date;
}

}  // namespace gui
}  // namespace phatbooks
