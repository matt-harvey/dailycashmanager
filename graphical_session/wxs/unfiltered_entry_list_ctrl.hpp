// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_unfiltered_entry_list_ctrl_hpp_2315717079823705
#define GUARD_unfiltered_entry_list_ctrl_hpp_2315717079823705

#include "entry_list_ctrl.hpp"
#include <wx/window.h>
#include <wx/gdicmn.h>

namespace phatbooks
{

// Begin forward declarations

class Account;

// End forward declarations

namespace gui
{

/**
 * An EntryListCtrl which is not filtered by Account or date.
 * Shows only \e actual (non-budget) and \e ordinary (non-draft) Entries.
 *
 * WARNING This may not end up getting used in the final version.
 */
class UnfilteredEntryListCtrl: public EntryListCtrl
{
public:
	UnfilteredEntryListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection
	);

	virtual ~UnfilteredEntryListCtrl();

private:
	virtual void do_set_non_date_columns(long p_row, Entry const& p_entry);
	virtual void do_insert_non_date_columns();
	virtual bool do_require_progress_log() const;
	virtual bool do_approve_entry(Entry const& p_entry) const;
	virtual void do_set_column_widths();
	virtual int do_get_comment_col_num() const;
	virtual int do_get_num_columns() const;
	virtual void do_update_for_amended(Account const& p_account);

	void adjust_account_column();

};  // class UnfilteredEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_unfiltered_entry_list_ctrl_hpp_2315717079823705
