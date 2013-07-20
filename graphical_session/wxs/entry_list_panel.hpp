#ifndef GUARD_entry_list_panel_hpp
#define GUARD_entry_list_panel_hpp

#include "entry.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/window.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class OrdinaryJournal;
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
 *
 * @todo Do we want the user to be able to view non-actual transactions?
 * If we do, then, among other things, the AccountCtrl will need to allow
 * the user to select Accounts of account_type::pure_envelope.
 *
 * @todo EntryListCtrl has zero height until such time as user
 * first clicks "Refresh". Various attempted workarounds for this
 * have been tried to no avail...
 */
class EntryListPanel: public wxPanel, private boost::noncopyable
{
public:
	EntryListPanel
	(	wxWindow* p_parent,
		PhatbooksDatabaseConnection& p_database_connection
	);

	void update_for_new(OrdinaryJournal const& p_journal);
	void update_for_amended(OrdinaryJournal const& p_journal);
	void update_for_deleted(std::vector<Entry::Id> const& p_doomed_ids);
	void selected_entries(std::vector<Entry>& out);

	// WARNING Hack... This should be private, but we need to call it from
	// TopPanel to ensure EntryListCtrl is properly sized, AFTER the
	// EntryListPanel has been constructed.
	void configure_entry_list_ctrl();

private:
	void on_refresh_button_click(wxCommandEvent& event);
	Account selected_account() const;
	boost::optional<boost::gregorian::date> selected_min_date() const;
	boost::optional<boost::gregorian::date> selected_max_date() const;
	
	static int const s_account_ctrl_id = wxID_HIGHEST + 1;
	static int const s_min_date_ctrl_id = s_account_ctrl_id + 1;
	static int const s_max_date_ctrl_id = s_min_date_ctrl_id + 1;
	static int const s_refresh_button_id = s_max_date_ctrl_id + 1;
	static int const s_entry_list_ctrl_id = s_refresh_button_id + 1;

	int m_next_row;

	wxGridBagSizer* m_top_sizer;
	AccountCtrl* m_account_ctrl;
	DateCtrl* m_min_date_ctrl;
	DateCtrl* m_max_date_ctrl;
	wxButton* m_refresh_button;
	EntryListCtrl* m_entry_list_ctrl;
	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class EntryListPanel

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_list_panel_hpp
