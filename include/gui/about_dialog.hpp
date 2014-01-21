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
#include <wx/generic/aboutdlgg.h>
#include <wx/string.h>
#include <wx/window.h>

namespace dcm
{
namespace gui
{

class AboutDialog: public wxGenericAboutDialog
{
public:

	// The fact we pass in p_developers and p_artists here is a bit
	// inconsistent in that it sidesteps/duplicates functionality already
	// present in wxAboutDialogInfo. But we need to be able to access this
	// information from within our customized AboutDialog, and this seems to be
	// the only way to do this.
	AboutDialog
	(	wxAboutDialogInfo const& p_info,
		wxWindow* p_parent,
		wxString const& p_developer_credits,
		wxString const& p_artist_credits,
		wxString const& p_brief_license_summary,
		wxString const& p_license_url
	);

	AboutDialog(AboutDialog const& rhs) = delete;
	AboutDialog(AboutDialog&& rhs) = delete;
	AboutDialog& operator=(AboutDialog const& rhs) = delete;
	AboutDialog& operator=(AboutDialog&& rhs) = delete;
	~AboutDialog();

	void DoAddCustomControls() override;

private:
	wxString m_developer_credits;
	wxString m_artist_credits;
	wxString m_brief_license_summary;
	wxString m_license_url;

};  // class AboutDialog

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_about_dialog_hpp_12818188785025006
