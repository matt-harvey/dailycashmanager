// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "draft_journal_naming_ctrl.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/dialog.h>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

DraftJournalNamingCtrl::DraftJournalNamingCtrl
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxDialog
	(	0,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxRESIZE_BORDER
	),
	m_top_sizer(0),
	m_database_connection(p_database_connection)
{
	m_top_sizer = new wxGridSizer(3, 1, 0, 0);
	SetSizer(m_top_sizer);

	// TODO Configure stuff	

	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
	CentreOnScreen();
}


}  // namespace gui
}  // namespace phatbooks
