#ifndef GUARD_welcome_dialog_hpp
#define GUARD_welcome_dialog_hpp

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

// End forward declarations

class WelcomeDialog: public wxDialog
{
public:

	WelcomeDialog(PhatbooksDatabaseConnection& p_database_connection);

	bool user_wants_new_file() const;

private:
	
	void configure_buttons();
	void on_new_file_button_click(wxCommandEvent& event);
	void on_existing_file_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	PhatbooksDatabaseConnection& m_database_connection;
	bool m_user_wants_new_file;
	wxGridSizer* m_top_sizer;
	wxButton* m_new_file_button;
	wxButton* m_existing_file_button;

	// TODO The cancel button looks ugly here. It would be better to have
	// a close box at the top right; however this was not showing when
	// I tried to add one using wxCLOSE_BOX style (at least not on
	// Fedora with Gnome). Hence the cancel button. Perhaps on Windows,
	// however, we can get the cancel button to work?
	wxButton* m_cancel_button;

	static int const s_new_file_button_id = wxID_HIGHEST + 1;
	static int const s_existing_file_button_id = wxID_HIGHEST + 2;

	DECLARE_EVENT_TABLE()

};  // WelcomeDialog


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_welcome_dialog_hpp
