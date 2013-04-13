#include "welcome_wizard.hpp"
#include "icon.xpm"
#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <cassert>
#include <wx/wizard.h>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

WelcomeWizard::WelcomeWizard
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizard
	(	0,
		wxID_ANY,
		wxString("Welcome!"),
		wxBitmap(icon_xpm),  // TODO Put a proper image here
		wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	),
	m_database_connection(p_database_connection)
{
	assert (!m_database_connection.is_valid());
}

void
WelcomeWizard::run()
{
	// TODO Implement
}



}  // namespace gui
}  // namesapce phatbooks


