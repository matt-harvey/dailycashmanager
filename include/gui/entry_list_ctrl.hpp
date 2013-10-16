/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_entry_list_ctrl_hpp_03525603377970682
#define GUARD_entry_list_ctrl_hpp_03525603377970682


#include "entry_table_iterator.hpp"
#include "reconciliation_list_panel.hpp"
#include "summary_datum.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <memory>
#include <unordered_set>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class Entry;
class DateParser;
class OrdinaryJournal;
class PhatbooksDatabaseConnection;

namespace gui
{

class ReconciliationEntryListCtrl;

// End forward declarations

/**
 * Displays a list of Entries. Derived classes must define various
 * pure virtual functions - and may override certain impure virtual functions
 * - to govern which Entries are displayed, and which columns of data are
 * displayed. However every EntryListCtrl will have the date of the Entry
 * showing in the first column, and should only ever show Entries from
 * OrdinaryJournals (not DraftJournals).
 *
 * @todo LOW PRIORITY Better document the interface with derived classes.
 */
class EntryListCtrl: public wxListCtrl
{
public:

	/**
	 * @returns a pointer to a heap-allocated EntryListCtrl, listing
	 * all and only the \e actual (non-budget) OrdinaryEntries stored in
	 * \e p_account.database_connection() which have \e p_account as their
	 * Account, and which lie between the date stored in p_maybe_min_date
	 * and the date stored in p_maybe_max_date, inclusive. If
	 * p_maybe_min_date is an uninitialized optional, then there is no
	 * minimum date; and if p_maybe_max_date is an uninitialized optional
	 * then there is no maximum date. Having said this, even if there is no
	 * minimum date, then the opening balance date may act as an effective
	 * minimum date...
	 */
	static EntryListCtrl* create_actual_ordinary_entry_list
	(	wxWindow* p_parent,
		wxSize const& p_size,
		sqloxx::Handle<Account> const& p_account,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> const& p_maybe_max_date =
			boost::optional<boost::gregorian::date>()
	);

	/**
	 * @returns a pointer to a heap-allocated EntryListCtrl, especially
	 * designed to facilitate balance sheet Account reconciliations.
	 *
	 * \e p_account should be handle to a balance sheet Account.
	 */
	static ReconciliationEntryListCtrl* create_reconciliation_entry_list
	(	ReconciliationListPanel* p_parent,
		wxSize const& p_size,
		sqloxx::Handle<Account> const& p_account,
		boost::gregorian::date const& p_min_date,
		boost::gregorian::date const& p_max_date
	);

	EntryListCtrl(EntryListCtrl const&) = delete;
	EntryListCtrl(EntryListCtrl&&) = delete;
	EntryListCtrl& operator=(EntryListCtrl const&) = delete;
	EntryListCtrl& operator=(EntryListCtrl&&) = delete;
	virtual ~EntryListCtrl();

	/**
	 * Update displayed entries to reflect that a \e p_journal has been newly
	 * posted (having not previously existed in the database).
	 */
	void update_for_new(sqloxx::Handle<OrdinaryJournal> const& p_journal);

	/**
	 * Update displayed entries to reflect that an already-saved
	 * OrdinaryJournal p_journal has just been amended, and the amendments
	 * saved.
	 */
	void update_for_amended(sqloxx::Handle<OrdinaryJournal> const& p_journal);

	void update_for_amended(sqloxx::Handle<Account> const& p_account);

	void update_for_new(sqloxx::Handle<Account> const& p_account);

	/**
	 * Update displayed entries to reflect that the Entries with IDs
	 * (as in Entry::id()) in p_doomed_ids have been deleted from the
	 * database.
	 */
	void update_for_deleted(std::vector<sqloxx::Id> const& p_doomed_ids);

	/**
	 * Populates \e out with handles to the currently selected Entries
	 * (if any).
	 */
	void selected_entries(std::vector<sqloxx::Handle<Entry> >& out);

	void scroll_to_bottom();

	std::vector<SummaryDatum> const& summary_data() const;

protected:
	EntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection
	);

	int num_columns() const;
	int date_col_num() const;
	void autosize_column_widths();
	void adjust_comment_column_to_fit();

	boost::gregorian::date date_displayed
	(	long p_row,
		DateParser const& p_parser
	) const;

	long row_for_date(boost::gregorian::date const& p_date) const;
	PhatbooksDatabaseConnection& database_connection();
	PhatbooksDatabaseConnection const& database_connection() const;

private:

	virtual bool do_require_progress_log() const = 0;
	virtual void do_insert_non_date_columns() = 0;
	virtual bool do_approve_entry
	(	sqloxx::Handle<Entry> const& p_entry
	) const = 0;
	virtual void do_set_non_date_columns
	(	long p_row,
		sqloxx::Handle<Entry> const& p_entry
	) = 0;
	virtual void do_set_column_widths() = 0;
	virtual int do_get_num_columns() const = 0;
	virtual int do_get_comment_col_num() const = 0;
	virtual std::unique_ptr<sqloxx::SQLStatement>
		do_create_entry_selector() = 0;

	virtual void do_update_for_amended
	(	sqloxx::Handle<Account> const& p_account
	);

	void on_item_activated(wxListEvent& event);

	virtual std::vector<SummaryDatum> const& do_get_summary_data() const;
	virtual void do_initialize_summary_data();
	virtual void do_process_candidate_entry_for_summary
	(	sqloxx::Handle<Entry> const& p_entry
	);
	virtual void do_process_removal_for_summary(long p_row);

	void set_column_widths();
	int scrollbar_width_allowance() const;

	 // To be called by factory functions prior to returning pointer to newly
	 // created EntryListCtrl.
	static void initialize(EntryListCtrl* p_entry_list_ctrl);

	void insert_columns();
	void insert_date_column();
	void populate();
	void process_push_candidate_entry(sqloxx::Handle<Entry> const& p_entry);

	// If p_row is set to -1 (as it is by default) and the candidate Entry
	// is "approved", then the row it will be inserted into will be determined
	// automatically; otherwise, the row will be given by p_row.
	void process_insertion_candidate_entry
	(	sqloxx::Handle<Entry> const& p_entry,
		long p_row = -1
	);

	// This inserts in correct date order (if p_row is -1) or at an explicitly
	// specified row (if p_row is non-negative). If p_row is less than -1,
	// then behaviour is undefined.
	void insert_entry(sqloxx::Handle<Entry> const& p_entry, long p_row = -1);
	
	void remove_if_present(sqloxx::Id p_entry_id);

	// This doesn't take care of sorting by date
	void push_back_entry(sqloxx::Handle<Entry> const& p_entry);

	// To remember which Entries have been added.
	typedef std::unordered_set<sqloxx::Id> IdSet;
	IdSet m_id_set;

	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class EntryListCtrl






}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_list_ctrl_hpp_03525603377970682
