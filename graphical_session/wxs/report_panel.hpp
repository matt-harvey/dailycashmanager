#ifndef GUARD_report_panel_hpp
#define GUARD_report_panel_hpp

#include <boost/noncopyable.hpp>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/window.h>

namespace phatbooks
{

// Begin forward declarations

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

private:
	void on_refresh_button_click(wxCommandEvent& event);

	static int const s_refresh_button_id = wxID_HIGHEST + 1;

	int m_next_row;

	wxGridBagSizer* m_top_sizer;
	DateCtrl* m_min_date_ctrl;
	DateCtrl* m_max_date_ctrl;
	wxButton* m_refresh_button;
	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class ReportPanel

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_report_panel_hpp
