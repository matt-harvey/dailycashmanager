// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_list_ctrl_hpp
#define GUARD_entry_list_ctrl_hpp


#include "account.hpp"
#include "entry_reader.hpp"
#include "entry.hpp"
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
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
 * @todo When filtering by Account:
 * 	If balance sheet Account, add column showing cumulative balance for
 * 	Account for all time up the point of the Entry, for each Entry shown.
 * 	We may also want to add a "reconciled balance" column.
 *  If P&L Account, add column showing cumulative total spent in the period
 *  shown.
 *  These changes will probably make it so complex to update for new
 *  Journals, deleted Journals etc, that we may no longer want to
 *  do these updates at all, but rather just leave it up to the user
 *  to click "Refresh" - or else just do a crude, slowish update
 *  by refreshing the whole thing each time there's a change. But note,
 *  if we leave it up to the user to click "Refresh", then there will be
 *  times when the display shows non-existent Entries. We will then have
 *  to intercept the user on those occasions when they try to edit a
 *  transaction by selecting one of the non-existent Entries.
 */
class EntryListCtrl: public wxListCtrl
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
	 * Update displayed entries to reflect that a \e p_journal has been newly
	 * posted (having not previously existed in the database).
	 */
	void update_for_new(OrdinaryJournal const& p_journal);

	/**
	 * Update displayed entries to reflect that an already-saved
	 * OrdinaryJournal p_journal has just been amended, and the amendments
	 * saved.
	 *
	 * @todo Implement this.
	 */
	void update_for_amended(OrdinaryJournal const& p_journal);

	void update_for_new(Account const& p_account);
	void update_for_amended(Account const& p_account);

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

private:

	EntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		EntryReader const& p_reader,
		PhatbooksDatabaseConnection& p_database_connection
	);

	EntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		Account const& p_account,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> const& p_maybe_max_date =
			boost::optional<boost::gregorian::date>()
	);

	void insert_columns();
	void set_column_widths();

	/**
	 * @param entry must be an Entry with an id.
	 *
	 * @todo HIGH PRIORITY This doesn't take care of sorting by date.
	 */
	void add_entry(Entry const& entry);

	/**
	 * @returns true if and only if p_entry falls within the filtering
	 * parameters for the EntryListCtrl.
	 *
	 * Note this does NOT filter for opening balance journal date.
	 */
	bool would_accept_entry(Entry const& p_entry) const;

	bool filtering_for_account() const;
	bool showing_reconciled_column() const;

	int date_col_num() const;
	int account_col_num() const;
	int comment_col_num() const;
	int amount_col_num() const;
	int reconciled_col_num() const;
	int num_columns() const;

	/**
	 * To remember which Entries have been added.
	 */
	typedef boost::unordered_set<Entry::Id> IdSet;
	IdSet m_id_set;

	PhatbooksDatabaseConnection& m_database_connection;
	boost::optional<Account> m_maybe_account;
	boost::gregorian::date m_min_date;
	boost::optional<boost::gregorian::date> m_maybe_max_date;
};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_list_ctrl_hpp
