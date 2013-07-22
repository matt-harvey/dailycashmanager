// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_list_ctrl_hpp
#define GUARD_entry_list_ctrl_hpp


#include "account.hpp"
#include "entry_reader.hpp"
#include "entry.hpp"
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <boost/noncopyable.hpp>
#include <boost/unordered_set.hpp>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class OrdinaryJournal;
class PhatbooksDatabaseConnection;

// End forward declarations

namespace gui
{

/**
 * Displays a list of Entries. Derived classes must defined various
 * pure virtual functions - and may override certain impure virtual functions
 * - to govern which Entries are displayed, and which columns of data are
 * displayed. However every EntryListCtrl will have the date of the Entry
 * showing in the first column, and should only ever show Entries from
 * OrdinaryJournals (not DraftJournals).
 *
 * @todo Better document the interface with derived classes.
 *
 * @todo HIGH PRIORITY The "update_for..." functions are now broken
 * for PLAccountEntryListCtrl and BSAccountEntryListCtrl, as they do not
 * adjust the accumulator column/s.
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
	long row_for_date(boost::gregorian::date const& p_date);
	PhatbooksDatabaseConnection& database_connection();

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
	virtual void do_accumulate(Entry const& p_entry);
	virtual void do_initialize_accumulation
	(	EntryReader::const_iterator it,
		EntryReader::const_iterator const& end
	);

	/**
	 * Should return a pointer to an EntryReader which reads Entries from
	 * database_connection() associated with OrdinaryJournals
	 * (not DraftJournals). By default this returns a pointer to an
	 * ActualOrdinaryEntryReader. The base class EntryListCtrl will take
	 * care of managing the memory of the returned pointer.
	 */
	virtual EntryReader* do_make_entry_reader() const;

	void set_column_widths();
	int scrollbar_width_allowance() const;

	 // To be called by factory functions prior to returning pointer to newly
	 // created EntryListCtrl.
	static void initialize(EntryListCtrl* p_entry_list_ctrl);

	void insert_columns();
	void insert_date_column();
	void populate();
	void process_push_candidate_entry(Entry const& p_entry);
	void process_insertion_candidate_entry(Entry const& p_entry);

	// This inserts in correct date order
	void insert_entry(Entry const& p_entry);

	// This doesn't take care of sorting by date
	void push_back_entry(Entry const& p_entry);

	// To remember which Entries have been added.
	typedef boost::unordered_set<Entry::Id> IdSet;
	IdSet m_id_set;

	PhatbooksDatabaseConnection& m_database_connection;

};






}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_list_ctrl_hpp
