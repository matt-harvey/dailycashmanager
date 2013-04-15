#include "welcome_dialog.hpp"
#include "application.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/dialog.h>

namespace phatbooks
{
namespace gui
{

WelcomeDialog::WelcomeDialog
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxDialog
	(	0,
		wxID_ANY,
		wxString("Welcome to ") +
			bstring_to_wx(Application::application_name()) +
			wxString("!"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDIALOG_NO_PARENT
	),
	m_database_connection(p_database_connection),
	m_new_file_button(0),
	m_existing_file_button(0)
{
	
	
}




}  // namespace gui
}  // namespace phatbooks

