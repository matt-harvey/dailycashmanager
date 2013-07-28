#include "report_panel.hpp"
#include "date_ctrl.hpp"
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(ReportPanel, wxPanel)
	EVT_BUTTON(s_refresh_button_id, ReportPanel::on_refresh_button_click)
END_EVENT_TABLE()

ReportPanel::ReportPanel
(	wxWindow* p_parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel(p_parent, wxID_ANY),
	m_next_row(0),
	m_top_sizer(0),
	m_min_date_ctrl(0),
	m_max_date_ctrl(0),
	m_refresh_button(0),
	m_database_connection(p_database_connection)
{
	// TODO HIGH PRIORITY Implement
}


void
ReportPanel::on_refresh_button_click(wxCommandEvent& event)
{
	// TODO HIGH PRIORITY Implement
}


}  // namespace gui
}  // namespace phatbooks
