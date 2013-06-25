// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_naming_ctrl_hpp
#define GUARD_draft_journal_naming_ctrl_hpp

#include <boost/noncopyable.hpp>
#include <wx/dialog.h>

namespace phatbooks
{
namespace gui
{

/**
 * Widget for extracting a name from the user, for a DraftJournal created
 * via the GUI, and then saving that DraftJournal providing the user
 * provides a valid name.
 */
class DraftJournalNamingCtrl: public wxDialog, private boost::noncopyable
{
public:

protected:

private:

};  // class DraftJournalNamingCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_draft_journal_naming_ctrl_hpp
