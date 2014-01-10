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

#ifndef GUARD_about_dialog_hpp_12818188785025006
#define GUARD_about_dialog_hpp_12818188785025006

#include <wx/aboutdlg.h>
#include <wx/event.h>
#include <wx/generic/aboutdlgg.h>
#include <wx/string.h>
#include <wx/window.h>
#include <vector>

namespace dcm
{
namespace gui
{

// begin forward declarations

class Button;

// end forward declarations

class AboutDialog: public wxGenericAboutDialog
{
public:
	AboutDialog(wxAboutDialogInfo const& p_info, wxWindow* p_parent);
	AboutDialog(AboutDialog const& rhs) = delete;
	AboutDialog(AboutDialog&& rhs) = delete;
	AboutDialog& operator=(AboutDialog const& rhs) = delete;
	AboutDialog& operator=(AboutDialog&& rhs) = delete;
	~AboutDialog();

	void DoAddCustomControls() override;

	// TODO LOW PRIORITY This is a bit inconsistent in that it sidesteps and
	// functionality already present in wxAboutDialogInfo. But we need to
	// be able to access this information from within our customized
	// AboutDialog, and this seems to be the only way to do this.
	void add_developer(wxString const& p_developer);
	void add_artist(wxString const& p_artist);
	void set_license(wxString const& p_license);

private:
	void on_developers_button_click(wxCommandEvent& event);
	void on_artists_button_click(wxCommandEvent& event);
	void on_license_button_click(wxCommandEvent& event);

	static int const s_developers_button_id = wxID_HIGHEST + 1;
	static int const s_artists_button_id = s_developers_button_id + 1;
	static int const s_license_button_id = s_artists_button_id + 1;
	Button* m_developers_button;
	Button* m_artists_button;
	Button* m_license_button;
	std::vector<wxString> m_developers;
	std::vector<wxString> m_artists;
	wxString m_license;

	DECLARE_EVENT_TABLE()

};  // class AboutDialog

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_about_dialog_hpp_12818188785025006
