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

#ifndef GUARD_draft_journal_list_ctrl_hpp_9173440862993449
#define GUARD_draft_journal_list_ctrl_hpp_9173440862993449

#include "draft_journal_table_iterator.hpp"
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class DraftJournal;
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
		DraftJournalTableIterator p_beg,
		DraftJournalTableIterator p_end,
		PhatbooksDatabaseConnection& p_database_connection
	);

	DraftJournalListCtrl(DraftJournalListCtrl const&) = delete;
	DraftJournalListCtrl(DraftJournalListCtrl&&) = delete;
	DraftJournalListCtrl& operator=(DraftJournalListCtrl const&) = delete;
	DraftJournalListCtrl& operator=(DraftJournalListCtrl&&) = delete;
	~DraftJournalListCtrl() = default;

	/**
	 * Populates \e out with all the DraftJournals currently
	 * selected by the user in the DraftJournalListCtrl.
	 */
	void selected_draft_journals
	(	std::vector<sqloxx::Handle<DraftJournal> >& out
	);

private:

	void on_item_activated(wxListEvent& event);

	void update
	(	DraftJournalTableIterator p_beg,
		DraftJournalTableIterator p_end
	);

	static int const s_name_col = 0;
	static int const s_frequency_col = s_name_col + 1;
	static int const s_next_date_col = s_frequency_col + 1;

	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class DraftJournalListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_draft_journal_list_ctrl_hpp_9173440862993449
