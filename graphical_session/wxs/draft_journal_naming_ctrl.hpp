// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_naming_ctrl_hpp
#define GUARD_draft_journal_naming_ctrl_hpp

#include "b_string.hpp"
#include <boost/noncopyable.hpp>
#include <wx/dialog.h>
#include <wx/sizer.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

// End forward declarations

namespace gui
{

/**
 * Widget for extracting a name from the user, for a DraftJournal created
 * via the GUI.
 */
class DraftJournalNamingCtrl: public wxDialog, private boost::noncopyable
{
public:
	
	DraftJournalNamingCtrl
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	BString draft_journal_name() const;

private:

	wxGridSizer* m_top_sizer;
	PhatbooksDatabaseConnection& m_database_connection;

};  // class DraftJournalNamingCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_draft_journal_naming_ctrl_hpp
