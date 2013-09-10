// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_list_ctrl_hpp
#define GUARD_draft_journal_list_ctrl_hpp

#include "draft_journal.hpp"
#include "draft_journal_table_iterator.hpp"
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

// End forward declarations

namespace gui
{

class DraftJournalListCtrl: public wxListCtrl
{
public:

	DraftJournalListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		UserDraftJournalTableIterator p_beg,
		UserDraftJournalTableIterator p_end,
		PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Populates \e out with all the DraftJournals currently
	 * selected by the user in the DraftJournalListCtrl.
	 */
	void selected_draft_journals(std::vector<DraftJournal>& out);

private:

	void on_item_activated(wxListEvent& event);

	void update
	(	UserDraftJournalTableIterator p_beg,
		UserDraftJournalTableIterator p_end
	);

	static int const s_name_col = 0;
	static int const s_frequency_col = s_name_col + 1;
	static int const s_next_date_col = s_frequency_col + 1;

	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class DraftJournalListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_draft_journal_list_ctrl_hpp
