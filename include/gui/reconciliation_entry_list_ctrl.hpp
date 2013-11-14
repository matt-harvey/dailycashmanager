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

#ifndef GUARD_reconciliation_entry_list_ctrl_hpp_7164053319564114
#define GUARD_reconciliation_entry_list_ctrl_hpp_7164053319564114

#include "filtered_entry_list_ctrl.hpp"
#include "reconciliation_list_panel.hpp"
#include "summary_datum.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <wx/gdicmn.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/window.h>
#include <memory>


namespace phatbooks
{

// begin forward declarations

class Account;
class Entry;

// end forward declarations

namespace gui
{

class ReconciliationEntryListCtrl: public FilteredEntryListCtrl
{
public:

	ReconciliationEntryListCtrl
	(	ReconciliationListPanel* p_parent,
		wxSize const& p_size,
		sqloxx::Handle<Account> const& p_account,
		boost::gregorian::date const& p_min_date,
		boost::gregorian::date const& p_max_date
	);

	ReconciliationEntryListCtrl(ReconciliationEntryListCtrl const&) = delete;
	ReconciliationEntryListCtrl(ReconciliationEntryListCtrl&&) = delete;
	ReconciliationEntryListCtrl& operator=
	(	ReconciliationEntryListCtrl const&
	) = delete;
	ReconciliationEntryListCtrl& operator=
	(	ReconciliationEntryListCtrl&&
	) = delete;
	virtual ~ReconciliationEntryListCtrl();

private:

	virtual void do_set_non_date_columns
	(	long p_row, sqloxx::Handle<Entry> const& p_entry
	) override;

	virtual void do_insert_non_date_columns() override;

	virtual bool do_approve_entry
	(	sqloxx::Handle<Entry> const& p_entry
	) const override;

	virtual int do_get_comment_col_num() const override;

	virtual int do_get_num_columns() const override;

	virtual std::vector<SummaryDatum> const&
		do_get_summary_data() const override;

	virtual void do_initialize_summary_data() override;

	virtual void do_process_candidate_entry_for_summary
	(	sqloxx::Handle<Entry> const& p_entry
	) override;

	virtual void do_process_removal_for_summary(long p_row) override;

	virtual std::unique_ptr<sqloxx::SQLStatement>
		do_create_entry_selector() override;

	void on_item_right_click(wxListEvent& event);

	jewel::Decimal amount_for_row(long p_row) const;

	// This duplicates FilteredEntryListCtrl, but is done for convenience and
	// efficiency to avoid having to dereference an optional.
	boost::gregorian::date max_date() const;
	boost::gregorian::date m_max_date;

	std::unique_ptr<std::vector<SummaryDatum> > m_summary_data;
	jewel::Decimal m_closing_balance;
	jewel::Decimal m_reconciled_closing_balance;

	DECLARE_EVENT_TABLE()

};  // class ReconciliationEntryListCtrl

	
}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_reconciliation_entry_list_ctrl_hpp_7164053319564114
