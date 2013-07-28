#include "report_panel.hpp"
#include "date.hpp"
#include "date_ctrl.hpp"
#include "sizing.hpp"
#include "string_set_validator.hpp"
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(ReportPanel, wxPanel)
	EVT_BUTTON(s_refresh_button_id, ReportPanel::on_refresh_button_click)
END_EVENT_TABLE()

ReportPanel::ReportPanel
(	wxWindow* p_parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel(p_parent, wxID_ANY),
	m_next_row(0),
	m_top_sizer(0),
	m_report_type_ctrl(0),
	m_min_date_ctrl(0),
	m_max_date_ctrl(0),
	m_refresh_button(0),
	m_database_connection(p_database_connection)
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);

	++m_next_row;
	
	configure_top();
	configure_bottom();
	
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Fit();
	Layout();
}

void
ReportPanel::configure_top()
{
	assert (m_top_sizer);

	// Labels
	wxStaticText* report_type_label =
		new wxStaticText(this, wxID_ANY, wxString(" Report type:"));
	m_top_sizer->Add(report_type_label, wxGBPosition(m_next_row, 1));
	wxStaticText* min_date_label =
		new wxStaticText(this, wxID_ANY, wxString(" From:"));
	m_top_sizer->Add(min_date_label, wxGBPosition(m_next_row, 2));
	wxStaticText* max_date_label =
		new wxStaticText(this, wxID_ANY, wxString(" To:"));
	m_top_sizer->Add(max_date_label, wxGBPosition(m_next_row, 3));

	++m_next_row;
	
	// Report type combobox
	wxArrayString report_type_names;
	report_type_names.Add(wxString("Balance sheet"));
	report_type_names.Add(wxString("Income and expenditure"));
	m_report_type_ctrl = new wxComboBox
	(	this,
		wxID_ANY,
		report_type_names[0],
		wxDefaultPosition,
		wxSize(large_width(), wxDefaultSize.y),
		wxArrayString(),
		wxCB_READONLY
	);
	for (size_t i = 0; i != report_type_names.GetCount(); ++i)
	{
		m_report_type_ctrl->Append(report_type_names[i]);
	}
	assert (!report_type_names.IsEmpty());
	m_report_type_ctrl->SetValue(report_type_names[0]);
	m_top_sizer->Add(m_report_type_ctrl, wxGBPosition(m_next_row, 1));

	// WARNING There is duplicated code between here and EntryListPanel,
	// and also to some extent between here and ReconciliationPanel.
	assert (m_report_type_ctrl);
	int const std_height = m_report_type_ctrl->GetSize().GetHeight();

	// Date range boxes

	bool const allow_blank_dates = true;
	m_min_date_ctrl = new DateCtrl
	(	this,
		s_min_date_ctrl_id,
		wxSize(medium_width(), std_height),
		today(),
		allow_blank_dates
	);
	m_top_sizer->Add(m_min_date_ctrl, wxGBPosition(m_next_row, 2));
	m_max_date_ctrl = new DateCtrl
	(	this,
		s_max_date_ctrl_id,
		wxSize(medium_width(), std_height),
		today(),
		allow_blank_dates
	);
	m_top_sizer->Add(m_max_date_ctrl, wxGBPosition(m_next_row, 3));

	// Refresh button
	m_refresh_button = new wxButton
	(	this,
		s_refresh_button_id,
		wxString("&Refresh"),
		wxDefaultPosition,
		m_max_date_ctrl->GetSize()
	);
	m_refresh_button->SetDefault();
	m_top_sizer->Add(m_refresh_button, wxGBPosition(m_next_row, 4));

	++m_next_row;

	return;
}

void
ReportPanel::configure_bottom()
{
	assert (m_top_sizer);
	// TODO HIGH PRIORITY Implement
	return;
}

void
ReportPanel::update_for_new(OrdinaryJournal const& p_journal)
{
	// TODO HIGH PRIORITY Implement
}

void
ReportPanel::update_for_amended(OrdinaryJournal const& p_journal)
{
	// TODO HIGH PRIORITY Implement
}

void
ReportPanel::update_for_new(Account const& p_account)
{
	// TODO HIGH PRIORITY Implement
}

void
ReportPanel::update_for_amended(Account const& p_account)
{
	// TODO HIGH PRIORITY Implement
}

void
ReportPanel::update_for_deleted(std::vector<Entry::Id> const& p_doomed_ids)
{
	// TODO HIGH PRIORITY Implement
}

void
ReportPanel::on_refresh_button_click(wxCommandEvent& event)
{
	// TODO HIGH PRIORITY Implement
}


}  // namespace gui
}  // namespace phatbooks
