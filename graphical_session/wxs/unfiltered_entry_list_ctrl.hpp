#ifndef GUARD_unfiltered_entry_list_ctrl_hpp
#define GUARD_unfiltered_entry_list_ctrl_hpp

#include "entry_list_ctrl.hpp"
#include <wx/window.h>
#include <wx/gdicmn.h>

namespace phatbooks
{

// Begin forward declarations

class EntryReader;

// End forward declarations

namespace gui
{

/**
 * An EntryListCtrl which is not filtered by Account or date.
 * Shows only \e actual (non-budget) and \e ordinary (non-draft) Entries.
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
	
	virtual bool do_require_progress_log() const;
	virtual void do_insert_columns();
	virtual bool do_approve_entry(Entry const& p_entry) const;
	virtual void do_push_entry(Entry const& p_entry);
	virtual void do_set_column_widths();

};  // class UnfilteredEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_unfiltered_entry_list_ctrl_hpp
