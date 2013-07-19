#ifndef GUARD_entry_list_panel_hpp
#define GUARD_entry_list_panel_hpp

#include "account.hpp"
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <wx/panel.h>
#include <wx/window.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class AccountCtrl;
class DateCtrl;
class EntryListCtrl;

// End forward declarations

/**
 * A panel consisting of an EntryListCtrl at the bottom, and widgets
 * at the top to enable the user to filter the displayed Entries for a
 * particular Account and/or date range.
 */
class EntryListPanel: public wxPanel, private boost::noncopyable
{
public:
	EntryListPanel
	(	wxWindow* p_parent,
		PhatbooksDatabaseConnection& p_database_connection
	);

private:
	boost::optional<Account> maybe_account() const;
	AccountCtrl* m_account_ctrl;
	DateCtrl* m_min_date_selector;
	DateCtrl* m_max_date_selector;
	EntryListCtrl* m_entry_list_ctrl;
	PhatbooksDatabaseConnection& m_database_connection;

};  // class EntryListPanel

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_list_panel_hpp
