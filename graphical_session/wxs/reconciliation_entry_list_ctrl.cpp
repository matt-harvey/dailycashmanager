// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "reconciliation_entry_list_ctrl.hpp"
#include "blank.xpm"
#include "commodity_handle.hpp"
#include "entry.hpp"
#include "entry_handle.hpp"
#include "filtered_entry_list_ctrl.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "ordinary_journal_handle.hpp"
#include "persistent_object_event.hpp"
#include "summary_datum.hpp"
#include "tick.xpm"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <memory>
#include <utility>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::SQLStatement;
using std::move;
using std::unique_ptr;
using std::vector;

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
	enum
	{
		blank_image_index = 0,  // to mark unreconciled Entries
		tick_image_index = 1    // to mark reconciled Entries
	};

}  // end anonymous namespace

ReconciliationEntryListCtrl::ReconciliationEntryListCtrl
(	ReconciliationListPanel* p_parent,
	wxSize const& p_size,
	AccountHandle const& p_account,
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
	m_closing_balance(0, p_account->commodity()->precision()),
	m_reconciled_closing_balance(0, p_account->commodity()->precision())
{
	JEWEL_LOG_TRACE();

	// TODO HIGH PRIORITY Images aren't showing under MSW.
	m_image_list = new wxImageList(0, 0);
	m_image_list->Add(wxBitmap(blank_xpm), *wxWHITE);  // must be 0th image
	m_image_list->Add(wxBitmap(tick_xpm), *wxWHITE);   // must be 1st image
	AssignImageList(m_image_list, wxIMAGE_LIST_SMALL);
}

void
ReconciliationEntryListCtrl::do_set_non_date_columns
(	long p_row,
	EntryHandle const& p_entry
)
{
	SetItem(p_row, comment_col_num(), p_entry->comment());
	SetItem
	(	p_row,
		amount_col_num(),
		finformat_wx
		(	p_entry->amount(),
			locale(),
			DecimalFormatFlags().clear(string_flags::dash_for_zero)
		)
	);
	SetItemColumnImage
	(	p_row,
		reconciled_col_num(),
		p_entry->is_reconciled()? tick_image_index: blank_image_index
	);
	JEWEL_ASSERT (num_columns() == 4);
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
		wxLIST_FORMAT_LEFT
	);
	JEWEL_ASSERT (num_columns() == 4);
	return;
}

bool
ReconciliationEntryListCtrl::do_approve_entry(EntryHandle const& p_entry) const
{
	if (p_entry->account() != account())
	{
		return false;
	}
	if (p_entry->date() > max_date())
	{
		return false;
	}
	if (p_entry->date() < min_date())
	{
		// We include unreconciled Entries even if they're prior to the
		// min_date().
		JEWEL_ASSERT
		(	(	p_entry->date() >
				database_connection().opening_balance_journal_date()
			) ||
			p_entry->is_reconciled()
		);
		return !p_entry->is_reconciled();
	}
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
	JEWEL_ASSERT (m_summary_data);
	JEWEL_ASSERT (!m_summary_data->empty());
	m_summary_data->at(0).set_amount(m_closing_balance);
	m_summary_data->at(1).set_amount(m_reconciled_closing_balance);
	return *m_summary_data;
}

void
ReconciliationEntryListCtrl::do_initialize_summary_data()
{
	m_closing_balance = Decimal(0, account()->commodity()->precision());
	m_reconciled_closing_balance =
		Decimal(0, account()->commodity()->precision());
	JEWEL_ASSERT (!m_summary_data);
	m_summary_data.reset(new std::vector<SummaryDatum>);
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
(	EntryHandle const& p_entry
)
{
	if (p_entry->account() != account())
	{	
		return;
	}
	if (p_entry->date() > max_date())
	{
		return;
	}
	jewel::Decimal const amount = p_entry->amount();
	m_closing_balance += amount;
	if (p_entry->is_reconciled()) m_reconciled_closing_balance += amount;
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
	if (item.GetImage() == tick_image_index)
	{
		m_reconciled_closing_balance -= amount;	
	}
	return;	
}

void
ReconciliationEntryListCtrl::on_item_right_click(wxListEvent& event)
{
	int const col = reconciled_col_num();
	sqloxx::Id const entry_id = event.GetData();	
	long const pos = event.GetIndex();
	JEWEL_ASSERT (FindItem(-1, entry_id) == pos);
	JEWEL_ASSERT (entry_id >= 0);
	JEWEL_ASSERT (GetItemData(pos) == static_cast<size_t>(entry_id));

	EntryHandle entry(database_connection(), entry_id);
	bool const old_reconciliation_status = entry->is_reconciled();
	entry->set_whether_reconciled(!old_reconciliation_status);
	
	SetItemColumnImage
	(	pos,
		col,
		entry->is_reconciled()? tick_image_index: blank_image_index
	);
	
	if (entry->is_reconciled())
	{
		m_reconciled_closing_balance += entry->amount();
	}
	else
	{
		m_reconciled_closing_balance -= entry->amount();
	}
	entry->save();

	JEWEL_ASSERT (entry->has_id());
	JEWEL_ASSERT (entry->id() == entry_id);
	PersistentObjectEvent::fire
	(	this,
		PHATBOOKS_RECONCILIATION_STATUS_EVENT,
		entry_id
	);

	ReconciliationListPanel* parent =
		dynamic_cast<ReconciliationListPanel*>(GetParent());
	JEWEL_ASSERT (parent);
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

unique_ptr<SQLStatement>
ReconciliationEntryListCtrl::do_create_entry_selector()
{
	unique_ptr<SQLStatement> ret
	(	create_date_ordered_actual_ordinary_entry_selector
		(	database_connection(),
			optional<gregorian::date>(),
			max_date(),
			account()
		)
	);
	return move(ret);
}

}  // namespace gui
}  // namespace phatbooks
