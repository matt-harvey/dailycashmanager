#ifndef GUARD_welcome_dialog_hpp
#define GUARD_welcome_dialog_hpp

#include <wx/button.h>
#include <wx/dialog.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

// End forward declarations

class WelcomeDialog:
	public wxDialog
{
public:
	WelcomeDialog(PhatbooksDatabaseConnection& p_database_connection);

private:
	PhatbooksDatabaseConnection& m_database_connection;
	wxButton* m_new_file_button;
	wxButton* m_existing_file_button;

};  // WelcomeDialog


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_welcome_dialog_hpp
