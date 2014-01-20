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
#include "gui/sizing.hpp"
#include <boost/filesystem.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <wx/hyperlink.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/utils.h>
#include <wx/window.h>

namespace filesystem = boost::filesystem;

namespace dcm
{
namespace gui
{

AboutDialog::AboutDialog
(	wxAboutDialogInfo const& p_info,
	wxWindow* p_parent,
	wxString const& p_developer_credits,
	wxString const& p_artist_credits
):
	m_developer_credits(p_developer_credits),
	m_artist_credits(p_artist_credits)
{
	Create(p_info, p_parent);
}

AboutDialog::~AboutDialog() = default;

void
AboutDialog::DoAddCustomControls()
{
	JEWEL_LOG_TRACE();
	auto developers_summary = new wxStaticText
	(	this,
		wxID_ANY,
		m_developer_credits,
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	AddControl
	(	developers_summary,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL | wxEXPAND
	);
	auto artists_summary = new wxStaticText
	(	this,
		wxID_ANY,
		m_artist_credits,
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	AddControl
	(	artists_summary,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL | wxEXPAND
	);
	auto license_summary = new wxStaticText
	(	this,
		wxID_ANY,
		App::application_name() +
			" is made available under the Apache License, Version 2.0, " +
			"which can be viewed at:",
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	AddControl
	(	license_summary,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL | wxEXPAND
	);
	auto license_url = new wxHyperlinkCtrl
	(	this,
		wxID_ANY,
		wxEmptyString,
		"http://www.apache.org/licenses/LICENSE-2.0",
		wxDefaultPosition,
		wxDefaultSize
	);
	AddControl
	(	license_url,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL | wxEXPAND
	);
	JEWEL_LOG_TRACE();
	return;
}

}  // namespace gui
}  // namespace dcm
