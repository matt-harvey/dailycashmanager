/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_welcome_dialog_hpp_920675095599459
#define GUARD_welcome_dialog_hpp_920675095599459

#include <wx/dialog.h>
#include <wx/sizer.h>

namespace dcm
{

// Begin forward declarations

class DcmDatabaseConnection;

namespace gui
{

class Button;

// End forward declarations

class WelcomeDialog: public wxDialog
{
public:

	WelcomeDialog(DcmDatabaseConnection& p_database_connection);

	WelcomeDialog(WelcomeDialog const&) = delete;
	WelcomeDialog(WelcomeDialog&&) = delete;
	WelcomeDialog& operator=(WelcomeDialog const&) = delete;
	WelcomeDialog& operator=(WelcomeDialog&&) = delete;
	virtual ~WelcomeDialog();

	bool user_wants_new_file() const;

private:
	
	void configure_buttons();
	void on_existing_file_button_click(wxCommandEvent& event);
	void on_new_file_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	DcmDatabaseConnection& m_database_connection;
	bool m_user_wants_new_file;
	wxGridSizer* m_top_sizer;
	Button* m_existing_file_button;
	Button* m_new_file_button;

	// TODO LOW PRIORITY The cancel button looks ugly here. It would be better
	// to have a close box at the top right; however this was not showing when
	// I tried to add one using wxCLOSE_BOX style (at least not on
	// Fedora with Gnome). Hence the cancel button. Perhaps on Windows,
	// however, we can get the cancel button to work?
	Button* m_cancel_button;

	static int const s_existing_file_button_id = wxID_HIGHEST + 1;
	static int const s_new_file_button_id = wxID_HIGHEST + 2;

	DECLARE_EVENT_TABLE()

};  // WelcomeDialog


}  // namespace gui
}  // namespace dcm

#endif  // GUARD_welcome_dialog_hpp_920675095599459
