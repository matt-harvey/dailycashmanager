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

// TODO On at least some builds, not all the text actually fits.

namespace filesystem = boost::filesystem;

namespace dcm
{
namespace gui
{

AboutDialog::AboutDialog
(	wxAboutDialogInfo const& p_info,
	wxWindow* p_parent,
	wxString const& p_developer_credits,
	wxString const& p_artist_credits,
	wxString const& p_brief_license_summary,
	wxString const& p_license_url
):
	m_developer_credits(p_developer_credits),
	m_artist_credits(p_artist_credits),
	m_brief_license_summary(p_brief_license_summary),
	m_license_url(p_license_url)
{
	Create(p_info, p_parent);
}

AboutDialog::~AboutDialog() = default;

void
AboutDialog::DoAddCustomControls()
{
	JEWEL_LOG_TRACE();
	AddText(m_developer_credits);
	AddText(m_artist_credits);
	AddText(m_brief_license_summary);
	AddText("A copy of the license can be obtained here:");
	auto license_url = new wxHyperlinkCtrl
	(	this,
		wxID_ANY,
		wxEmptyString,
		m_license_url,
		wxDefaultPosition,
		wxDefaultSize
	);
	AddControl(license_url, wxEXPAND);
	JEWEL_LOG_TRACE();
	return;
}

}  // namespace gui
}  // namespace dcm
