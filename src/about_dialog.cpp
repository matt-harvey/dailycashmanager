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

#include "gui/about_dialog.hpp"
#include "app.hpp"
#include "gui/button.hpp"
#include "gui/sizing.hpp"
#include <boost/filesystem.hpp>
#include <jewel/assert.hpp>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/utils.h>
#include <wx/window.h>
#include <vector>

namespace filesystem = boost::filesystem;

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(AboutDialog, wxGenericAboutDialog)
	EVT_BUTTON
	(	s_developers_button_id,
		AboutDialog::on_developers_button_click
	)
	EVT_BUTTON
	(	s_artists_button_id,
		AboutDialog::on_artists_button_click
	)
	EVT_BUTTON
	(	s_license_button_id,
		AboutDialog::on_license_button_click
	)
END_EVENT_TABLE()

AboutDialog::AboutDialog(wxAboutDialogInfo const& p_info, wxWindow* p_parent):
	m_developers_button(nullptr),
	m_artists_button(nullptr),
	m_license_button(nullptr)
{
	Create(p_info, p_parent);
}

AboutDialog::~AboutDialog() = default;

void
AboutDialog::DoAddCustomControls()
{
	m_developers_button = new Button
	(	this,
		s_developers_button_id,
		wxString("Developers"),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	AddControl(m_developers_button, wxSizerFlags().Expand());
	m_artists_button = new Button
	(	this,
		s_artists_button_id,
		wxString("Artists"),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	AddControl(m_artists_button, wxSizerFlags().Expand());
	m_license_button = new Button
	(	this,
		s_license_button_id,
		wxString("License"),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	AddControl(m_license_button, wxSizerFlags().Expand());
	return;
}

void
AboutDialog::add_developer(wxString const& p_developer)
{
	m_developers.push_back(p_developer);
	return;
}

void
AboutDialog::add_artist(wxString const& p_artist)
{
	m_artists.push_back(p_artist);
	return;
}

void
AboutDialog::on_developers_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused param.
	wxString msg;
	for (auto const& developer: m_developers)
	{
		msg += developer;
		msg += "\n";
	}
	wxMessageBox(msg, wxString("Developers"));
	return;
}

void
AboutDialog::on_artists_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused param.
	wxString msg;
	for (auto const& artist: m_artists)
	{
		msg += artist;
		msg += "\n";
	}
	wxMessageBox(msg, wxString("Artists"));
	return;
}

void
AboutDialog::on_license_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused param.
	wxLaunchDefaultBrowser(App::user_guide_url() + "/LICENSE.html");
	return;
}

}  // namespace gui
}  // namespace dcm
