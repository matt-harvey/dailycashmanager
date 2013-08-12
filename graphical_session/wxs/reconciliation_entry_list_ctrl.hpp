// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_reconciliation_entry_list_ctrl_hpp
#define GUARD_reconciliation_entry_list_ctrl_hpp

#include "filtered_entry_list_ctrl.hpp"
#include "reconciliation_list_panel.hpp"
#include "summary_datum.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <wx/window.h>


namespace phatbooks
{

// Begin forward declarations

class Account;
class Entry;

// End forward declarations


namespace gui
{

/**
 * @todo What about effect on opening balance etc. of an Entry being deleted
 * that is prior to the min_date? This shouldn't matter because the user
 * shouldn't be able to edit an Entry that is reconciled; and if it is
 * unreconciled it will be in the list... but make sure this is OK.
 *
 * @todo HIGH PRIORITY In ReconciliationPanel, and in
 * ReconciliationEntryListCtrl, we need to update rows for changes in
 * reconciliation status when required. This should be done separately
 * from the way we update for Journal amendments.
 */
class ReconciliationEntryListCtrl: public FilteredEntryListCtrl
{
public:

	ReconciliationEntryListCtrl
	(	ReconciliationListPanel* p_parent,
		wxSize const& p_size,
		Account const& p_account,
		boost::gregorian::date const& p_min_date,
		boost::gregorian::date const& p_max_date
	);

	virtual ~ReconciliationEntryListCtrl();

private:
	virtual void do_set_non_date_columns(long p_row, Entry const& p_entry);
	virtual void do_insert_non_date_columns();
	virtual bool do_approve_entry(Entry const& p_entry) const;
	virtual int do_get_comment_col_num() const;
	virtual int do_get_num_columns() const;

	virtual std::vector<SummaryDatum> const& do_get_summary_data() const;
	virtual void do_initialize_summary_data();
	virtual void do_process_candidate_entry_for_summary(Entry const& p_entry);
	virtual void do_process_removal_for_summary(long p_row);

	void on_item_right_click(wxListEvent& event);

	jewel::Decimal amount_for_row(long p_row) const;

	// This duplicates FilteredEntryListCtrl, but is done for ease and
	// efficiency to avoid having to deference an optional.
	boost::gregorian::date m_max_date;
	boost::gregorian::date max_date() const
	{
		return m_max_date;
	}

	std::vector<SummaryDatum>* m_summary_data;
	jewel::Decimal m_closing_balance;
	jewel::Decimal m_reconciled_closing_balance;

	DECLARE_EVENT_TABLE()

};  // class ReconciliationEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_reconciliation_entry_list_ctrl_hpp
