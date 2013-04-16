#ifndef GUARD_welcome_dialog_hpp
#define GUARD_welcome_dialog_hpp

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
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
	boost::filesystem::path get_filepath() const;
private:
	
	void configure_buttons();
	void on_new_file_button_click(wxCommandEvent& event);
	void on_existing_file_button_click(wxCommandEvent& event);

	PhatbooksDatabaseConnection& m_database_connection;
	boost::optional<boost::filesystem::path> m_filepath;
	wxGridSizer* m_top_sizer;
	wxButton* m_new_file_button;
	wxButton* m_existing_file_button;
	static int const s_new_file_button_id = wxID_HIGHEST + 1;
	static int const s_existing_file_button_id = wxID_HIGHEST + 2;

	DECLARE_EVENT_TABLE()

};  // WelcomeDialog


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_welcome_dialog_hpp
