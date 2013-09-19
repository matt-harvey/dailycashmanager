// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "report_panel.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "date.hpp"
#include "date_ctrl.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "report.hpp"
#include "sizing.hpp"
#include "string_set_validator.hpp"
#include <jewel/assert.hpp>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <utility>

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(ReportPanel, wxPanel)
	EVT_BUTTON(s_run_button_id, ReportPanel::on_run_button_click)
END_EVENT_TABLE()

namespace
{
	wxString balance_sheet_report_name()
	{
		return wxString("Balance sheet");
	}
	wxString pl_report_name()
	{
		return wxString("Income and expenditure");
	}

}  // end anonymous namespace

ReportPanel::ReportPanel
(	wxWindow* p_parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel(p_parent, wxID_ANY),
	m_next_row(0),
	m_client_size_aux(0),
	m_text_ctrl_height(0),
	m_top_sizer(0),
	m_report_type_ctrl(0),
	m_min_date_ctrl(0),
	m_max_date_ctrl(0),
	m_run_button(0),
	m_report(0),
	m_database_connection(p_database_connection)
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);

	++m_next_row;
	
	configure_top();
	// configure_bottom();  Don't run till user clicks.
	
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Fit();
	Layout();
}

void
ReportPanel::configure_top()
{
	JEWEL_ASSERT (m_top_sizer);

	// Labels
	wxStaticText* report_type_label =
		new wxStaticText(this, wxID_ANY, wxString(" Report type:"));
	m_top_sizer->Add(report_type_label, wxGBPosition(m_next_row, 1));
	wxStaticText* min_date_label =
		new wxStaticText(this, wxID_ANY, wxString(" From:"));
	m_text_ctrl_height = min_date_label->GetSize().GetY();
	m_top_sizer->Add(min_date_label, wxGBPosition(m_next_row, 2));
	wxStaticText* max_date_label =
		new wxStaticText(this, wxID_ANY, wxString(" To:"));
	m_top_sizer->Add(max_date_label, wxGBPosition(m_next_row, 3));

	++m_next_row;
	
	// Report type combobox
	wxArrayString report_type_names;
	report_type_names.Add(balance_sheet_report_name());
	report_type_names.Add(pl_report_name());
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
	JEWEL_ASSERT (!report_type_names.IsEmpty());
	m_report_type_ctrl->SetValue(report_type_names[0]);
	m_top_sizer->Add(m_report_type_ctrl, wxGBPosition(m_next_row, 1));

	// WARNING There is duplicated code between here and EntryListPanel,
	// and also to some extent between here and ReconciliationPanel.
	JEWEL_ASSERT (m_report_type_ctrl);
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
	m_run_button = new wxButton
	(	this,
		s_run_button_id,
		wxString("&Run"),
		wxDefaultPosition,
		m_max_date_ctrl->GetSize()
	);
	m_run_button->SetDefault();
	m_top_sizer->Add(m_run_button, wxGBPosition(m_next_row, 4));

	++m_next_row;

	return;
}

void
ReportPanel::configure_bottom()
{
	// TODO Can we make this a bit more efficient by, instead of creating a
	// whole new Report, calling a generate() or regenerate() method
	// on the existing Report?
	
	JEWEL_ASSERT (m_top_sizer);
	if (m_client_size_aux < 100)  // WARNING Ugly hack
	{
		m_client_size_aux = GetClientSize().GetY();
	}
	int const num_extra_rows = 2;
	JEWEL_ASSERT (m_min_date_ctrl);
	int height_aux =
		m_client_size_aux -
		m_min_date_ctrl->GetSize().GetY() * num_extra_rows -
		standard_gap() * (num_extra_rows + 1) -
		standard_border() * 2;

#	ifdef JEWEL_ON_WINDOWS
		height_aux -= standard_gap() * (num_extra_rows + 1);
#	endif

	Report* temp = Report::create
	(	this,
		wxSize
		(	large_width() + medium_width() * 3 + standard_gap() * 3,
			height_aux
		),
		selected_account_super_type(),
		m_database_connection,
		m_min_date_ctrl->date(),
		m_max_date_ctrl->date()
	);
	using std::swap;
	swap(temp, m_report);
	if (temp)
	{
		m_top_sizer->Detach(temp);
		temp->Destroy();
		temp = 0;
		--m_next_row;
	}
	m_report->generate();
	m_top_sizer->
		Add(m_report, wxGBPosition(m_next_row, 1), wxGBSpan(1, 4));
	// m_top_sizer->Fit(this);
	// m_top_sizer->SetSizeHints(this);
	// Fit();
	Layout();
	
	++m_next_row;

	return;
}

account_super_type::AccountSuperType
ReportPanel::selected_account_super_type() const
{
	JEWEL_ASSERT (m_report_type_ctrl);
	wxString const report_name = m_report_type_ctrl->GetValue();
	if (report_name == balance_sheet_report_name())
	{
		return account_super_type::balance_sheet;
	}
	JEWEL_ASSERT (report_name == pl_report_name());
	return account_super_type::pl;
}

void
ReportPanel::update_for_new(OrdinaryJournal const& p_journal)
{
	JEWEL_ASSERT (m_report);
	m_report->update_for_new(p_journal);
	return;
}

void
ReportPanel::update_for_amended(OrdinaryJournal const& p_journal)
{
	JEWEL_ASSERT (m_report);
	m_report->update_for_amended(p_journal);
	return;
}

void
ReportPanel::update_for_new(Account const& p_account)
{
	JEWEL_ASSERT (m_report);
	m_report->update_for_new(p_account);
	return;
}

void
ReportPanel::update_for_amended(Account const& p_account)
{
	JEWEL_ASSERT (m_report);
	m_report->update_for_amended(p_account);
	return;
}

void
ReportPanel::update_for_deleted(std::vector<Entry::Id> const& p_doomed_ids)
{
	JEWEL_ASSERT (m_report);
	m_report->update_for_deleted(p_doomed_ids);
	return;
}

void
ReportPanel::on_run_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	configure_bottom();
	JEWEL_ASSERT (m_run_button);
	m_run_button->SetLabel("&Refresh");
	return;
}


}  // namespace gui
}  // namespace phatbooks
