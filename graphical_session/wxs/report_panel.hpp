#ifndef GUARD_report_panel_hpp
#define GUARD_report_panel_hpp

#include "entry.hpp"
#include <boost/noncopyable.hpp>
#include <wx/combobox.h>
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

class DateCtrl;

// End forward declarations

/**
 * Panel for holding date-filtered balance sheet and profit-and-loss
 * reports.
 */
class ReportPanel: public wxPanel, private boost::noncopyable
{
public:
	ReportPanel
	(	wxWindow* p_parent,
		PhatbooksDatabaseConnection& p_database_connection
	);

	void update_for_new(OrdinaryJournal const& p_journal);
	void update_for_amended(OrdinaryJournal const& p_journal);
	void update_for_new(Account const& p_account);
	void update_for_amended(Account const& p_account);
	void update_for_deleted(std::vector<Entry::Id> const& p_doomed_ids);

private:
	void on_refresh_button_click(wxCommandEvent& event);

	void configure_top();
	void configure_bottom();

	static int const s_min_date_ctrl_id = wxID_HIGHEST + 1;
	static int const s_max_date_ctrl_id = s_min_date_ctrl_id + 1;
	static int const s_refresh_button_id = s_max_date_ctrl_id + 1;

	int m_next_row;

	wxGridBagSizer* m_top_sizer;
	wxComboBox* m_report_type_ctrl;
	DateCtrl* m_min_date_ctrl;
	DateCtrl* m_max_date_ctrl;
	wxButton* m_refresh_button;
	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class ReportPanel

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_report_panel_hpp
