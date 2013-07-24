#include "reconciliation_entry_list_ctrl.hpp"
#include "entry.hpp"
#include "filtered_entry_list_ctrl.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "ordinary_journal.hpp"
#include "summary_datum.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using std::vector;

// For debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

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
(	wxWindow* p_parent,
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
	m_closing_balance(0, p_account.commodity().precision()),
	m_reconciled_closing_balance(0, p_account.commodity().precision())
{
}

ReconciliationEntryListCtrl::~ReconciliationEntryListCtrl()
{
}

void
ReconciliationEntryListCtrl::do_set_non_date_columns
(	long p_row,
	Entry const& p_entry
)
{
	SetItem
	(	p_row,
		comment_col_num(),
		bstring_to_wx(p_entry.comment())
	);
	SetItem
	(	p_row,
		amount_col_num(),
		finformat_wx(p_entry.amount(), locale(), false)
	);
	SetItem
	(	p_row,
		reconciled_col_num(),
		(p_entry.is_reconciled()? wxString("Y"): wxString("N"))
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

vector<SummaryDatum>
ReconciliationEntryListCtrl::do_get_summary_data() const
{
	vector<SummaryDatum> ret;
	SummaryDatum a
	(	wxString("Closing balance"),
		m_closing_balance
	);
	SummaryDatum b
	(	wxString("Reconciled balance"),
		m_reconciled_closing_balance
	);
	ret.push_back(a);
	ret.push_back(b);
	return ret;
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
	Decimal const amount = amount_for_row(p_row);
	m_closing_balance -= amount;
#	ifndef NDEBUG
		wxListItem item;
		item.SetId(p_row);
		item.SetColumn(reconciled_col_num());
		GetItem(item);
		assert (item.GetText() == wxString("N"));
		// as we don't allow user to delete reconciled Entries.
		// so we don't need to adjust m_reconciled_closing_balance here.
#	endif
	return;	
}

Decimal
ReconciliationEntryListCtrl::amount_for_row(long p_row) const
{
	JEWEL_DEBUG_LOG << "Calling amount_for_row(long p_row) with p_row of: "
	                << p_row
					<< endl;
	wxListItem item;
	item.SetId(p_row);
	item.SetColumn(amount_col_num());
	GetItem(item);
	Decimal ret = wx_to_decimal(item.GetText(), locale());
	JEWEL_DEBUG_LOG << "Returning amount of: " << ret << endl;
	return ret;
}


}  // namespace gui
}  // namespace phatbooks
