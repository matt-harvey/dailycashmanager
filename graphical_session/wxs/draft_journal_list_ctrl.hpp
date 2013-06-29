#ifndef GUARD_draft_journal_list_ctrl_hpp
#define GUARD_draft_journal_list_ctrl_hpp

#include "draft_journal.hpp"
#include <wx/gdicmn.h>
#include <wx/listctrl.h>

namespace phatbooks
{

// Begin forward declarations

class UserDraftJournalReader;

// End forward declarations

namespace gui
{

class DraftJournalListCtrl: public wxListCtrl
{
public:

	DraftJournalListCtrl
	(	wxWindow* p_parent,
		wxSize const& p_size,
		UserDraftJournalReader const& p_reader
	);

private:

	void update(UserDraftJournalReader const& p_reader);

	static int const s_name_col = 0;
	static int const s_frequency_col = s_name_col + 1;
	static int const s_next_date_col = s_frequency_col + 1;

};  // class DraftJournalListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_draft_journal_list_ctrl_hpp
