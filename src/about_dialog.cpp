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
#include <wx/aboutdlg.h>
#include <wx/generic/aboutdlgg.h>
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
	wxString const& p_credits
):
	m_credits(p_credits)
{
	Create(p_info, p_parent);
}

AboutDialog::~AboutDialog() = default;

void
AboutDialog::DoAddCustomControls()
{
	JEWEL_LOG_TRACE();
	AddText(m_credits);
	AddCollapsiblePane
	(	wxString("License"),
		wxString
		(	"Licensed under the Apache License, Version 2.0 (the \"License\");\n"
			"you may not use this software except in compliance with the License.\n"
			"You may obtain a copy of the License at\n"
			"\n"
			"http://www.apache.org/licenses/LICENSE-2.0\n"
			"\n"
			"Unless required by applicable law or agreed to in writing, software\n"
			"distributed under the License is distributed on an \"AS IS\" BASIS,\n"
			"WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
			"See the License for the specific language governing permissions and\n"
			"limitations under the License\n"
			"\n"
		)
	);
	JEWEL_LOG_TRACE();
	return;
}

}  // namespace gui
}  // namespace dcm
