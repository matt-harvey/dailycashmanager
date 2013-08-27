// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_list_ctrl_hpp
#define GUARD_entry_list_ctrl_hpp


#include "account.hpp"
#include "entry_reader.hpp"
#include "entry.hpp"
#include "reconciliation_list_panel.hpp"
#include "summary_datum.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_set.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class OrdinaryJournal;
class PhatbooksDatabaseConnection;

namespace gui
{

class ReconciliationEntryListCtrl;

// End forward declarations

/**
 * Displays a list of Entries. Derived classes must defined various
 * pure virtual functions - and may override certain impure virtual functions
 * - to govern which Entries are displayed, and which columns of data are
 * displayed. However every EntryListCtrl will have the date of the Entry
 * showing in the first column, and should only ever show Entries from
 * OrdinaryJournals (not DraftJournals).
 *
 * @todo Better document the interface with derived classes.
 */
class EntryListCtrl: public wxListCtrl, private boost::noncopyable
{
public:

	/**
	 * @returns a pointer to a heap-allocated EntryListCtrl, listing
	 * all and only the \e actual (non-budget) OrdinaryEntries stored in \e
	 * dbc. The client does not need to take care of the memory - the memory
	 * is taken care of by the parent window.
	 */
	static EntryListCtrl* create_actual_ordinary_entry_list
	(	wxWindow* parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& dbc
	);

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
		Account const& p_account,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> const& p_maybe_max_date =
			boost::optional<boost::gregorian::date>()
	);

	/**
	 * @returns a pointer to a heap-allocated EntryListCtrl, especially
	 * designed to facilitate balance sheet Account reconciliations.
	 *
	 * \e p_account should be a balance sheet Account.
	 */
	static ReconciliationEntryListCtrl* create_reconciliation_entry_list
	(	ReconciliationListPanel* p_parent,
		wxSize const& p_size,
		Account const& p_account,
		boost::gregorian::date const& p_min_date,
		boost::gregorian::date const& p_max_date
	);

	virtual ~EntryListCtrl();

	/**
	 * Update displayed entries to reflect that a \e p_journal has been newly
	 * posted (having not previously existed in the database).
	 */
	void update_for_new(OrdinaryJournal const& p_journal);

	/**
	 * Update displayed entries to reflect that an already-saved
	 * OrdinaryJournal p_journal has just been amended, and the amendments
	 * saved.
	 */
	void update_for_amended(OrdinaryJournal const& p_journal);

	void update_for_amended(Account const& p_account);

	void update_for_new(Account const& p_account);

	/**
	 * Update displayed entries to reflect that the Entries with IDs
	 * (as in Entry::id()) in p_doomed_ids have been deleted from the
	 * database.
	 */
	void update_for_deleted(std::vector<Entry::Id> const& p_doomed_ids);

	/**
	 * Populates \e out with the currently selected Entries (if any).
	 */
	void selected_entries(std::vector<Entry>& out);

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
	boost::gregorian::date date_displayed(long p_row) const;
	long row_for_date(boost::gregorian::date const& p_date) const;
	PhatbooksDatabaseConnection& database_connection();
	PhatbooksDatabaseConnection const& database_connection() const;

private:

	virtual bool do_require_progress_log() const = 0;
	virtual void do_insert_non_date_columns() = 0;
	virtual bool do_approve_entry(Entry const& p_entry) const = 0;
	virtual void do_set_non_date_columns
	(	long p_row,
		Entry const& p_entry
	) = 0;
	virtual void do_set_column_widths() = 0;
	virtual int do_get_num_columns() const = 0;
	virtual int do_get_comment_col_num() const = 0;
	virtual void do_update_for_amended(Account const& p_account);

	void on_item_activated(wxListEvent& event);

	/**
	 * Should return a pointer to an EntryReader which reads Entries from
	 * database_connection() associated with OrdinaryJournals
	 * (not DraftJournals). By default this returns a pointer to an
	 * ActualOrdinaryEntryReader. The base class EntryListCtrl will take
	 * care of managing the memory of the returned pointer. The reader should
	 * also read Entries in date order - or things could get messy.
	 */
	virtual EntryReader* do_make_entry_reader() const;

	virtual std::vector<SummaryDatum> const& do_get_summary_data() const;
	virtual void do_initialize_summary_data();
	virtual void do_process_candidate_entry_for_summary(Entry const& p_entry);
	virtual void do_process_removal_for_summary(long p_row);

	void set_column_widths();
	int scrollbar_width_allowance() const;

	 // To be called by factory functions prior to returning pointer to newly
	 // created EntryListCtrl.
	static void initialize(EntryListCtrl* p_entry_list_ctrl);

	void insert_columns();
	void insert_date_column();
	void populate();
	void process_push_candidate_entry(Entry const& p_entry);

	// If p_row is set to -1 (as it is by default) and the candidate Entry
	// is "approved", then the row it will be inserted into will be determined
	// automatically; otherwise, the row will be given by p_row.
	void process_insertion_candidate_entry
	(	Entry const& p_entry,
		long p_row = -1
	);

	// This inserts in correct date order (if p_row is -1) or at an explicitly
	// specified row (if p_row is non-negative). If p_row is less than -1,
	// then behaviour is undefined.
	void insert_entry(Entry const& p_entry, long p_row = -1);
	
	void remove_if_present(Entry::Id p_entry_id);

	// This doesn't take care of sorting by date
	void push_back_entry(Entry const& p_entry);

	// To remember which Entries have been added.
	typedef boost::unordered_set<Entry::Id> IdSet;
	IdSet m_id_set;

	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class EntryListCtrl






}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_list_ctrl_hpp
