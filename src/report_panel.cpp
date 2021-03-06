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

#include "gui/report_panel.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "date.hpp"
#include "ordinary_journal.hpp"
#include "dcm_database_connection.hpp"
#include "gui/button.hpp"
#include "gui/combo_box.hpp"
#include "gui/date_ctrl.hpp"
#include "gui/report.hpp"
#include "gui/sizing.hpp"
#include "gui/string_set_validator.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <utility>

using sqloxx::Handle;

namespace gregorian = boost::gregorian;

namespace dcm
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
(   wxWindow* p_parent,
    DcmDatabaseConnection& p_database_connection
):
    wxPanel(p_parent, wxID_ANY),
    m_next_row(0),
    m_client_size_aux(0),
    m_text_ctrl_height(0),
    m_top_sizer(nullptr),
    m_report_type_ctrl(nullptr),
    m_min_date_ctrl(nullptr),
    m_max_date_ctrl(nullptr),
    m_run_button(nullptr),
    m_report(nullptr),
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

ReportPanel::~ReportPanel()
{
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
    m_report_type_ctrl = new ComboBox
    (   this,
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

    // TODO LOW PRIORITY Factor out duplicated code between here and
    // EntryListPanel - and perhaps also duplicated code between here and
    // ReconciliationPanel.
    JEWEL_ASSERT (m_report_type_ctrl);
    int const std_height = m_report_type_ctrl->GetSize().GetHeight();

    // Date range boxes

    auto const allow_blank_dates = true;
    auto const default_max_date = today();
    auto const default_min_date = default_max_date - gregorian::date_duration(6);
    m_min_date_ctrl = new DateCtrl
    (   this,
        s_min_date_ctrl_id,
        wxSize(medium_width(), std_height),
        default_min_date,
        allow_blank_dates
    );
    m_top_sizer->Add(m_min_date_ctrl, wxGBPosition(m_next_row, 2));
    m_max_date_ctrl = new DateCtrl
    (   this,
        s_max_date_ctrl_id,
        wxSize(medium_width(), std_height),
        default_max_date,
        allow_blank_dates
    );
    m_top_sizer->Add(m_max_date_ctrl, wxGBPosition(m_next_row, 3));

    // Refresh button
    m_run_button = new Button
    (   this,
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
    JEWEL_ASSERT (m_top_sizer);
    if (m_client_size_aux == 0)
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

#   ifdef JEWEL_ON_WINDOWS
        height_aux -= standard_gap() * (num_extra_rows + 1);
#   endif

    Report* temp = Report::create
    (   this,
        wxSize
        (   large_width() + medium_width() * 3 + standard_gap() * 3,
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
    JEWEL_ASSERT (m_report);
    m_report->generate();
    m_top_sizer->Add(m_report, wxGBPosition(m_next_row, 1), wxGBSpan(1, 4));
    // m_top_sizer->Fit(this);
    // m_top_sizer->SetSizeHints(this);
    // Fit();
    Layout();
    
    ++m_next_row;

    return;
}

AccountSuperType
ReportPanel::selected_account_super_type() const
{
    JEWEL_ASSERT (m_report_type_ctrl);
    wxString const report_name = m_report_type_ctrl->GetValue();
    if (report_name == balance_sheet_report_name())
    {
        return AccountSuperType::balance_sheet;
    }
    JEWEL_ASSERT (report_name == pl_report_name());
    return AccountSuperType::pl;
}

void
ReportPanel::update_for_new(Handle<OrdinaryJournal> const& p_journal)
{
    if (m_report) m_report->update_for_new(p_journal);
    return;
}

void
ReportPanel::update_for_amended(Handle<OrdinaryJournal> const& p_journal)
{
    if (m_report) m_report->update_for_amended(p_journal);
    return;
}

void
ReportPanel::update_for_new(Handle<Account> const& p_account)
{
    if (m_report) m_report->update_for_new(p_account);
    return;
}

void
ReportPanel::update_for_amended(Handle<Account> const& p_account)
{
    if (m_report) m_report->update_for_amended(p_account);
    return;
}

void
ReportPanel::update_for_amended_budget(Handle<Account> const& p_account)
{
    if (m_report) m_report->update_for_amended_budget(p_account);
    return;
}

void
ReportPanel::update_for_deleted(std::vector<sqloxx::Id> const& p_doomed_ids)
{
    if (m_report) m_report->update_for_deleted(p_doomed_ids);
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
}  // namespace dcm
