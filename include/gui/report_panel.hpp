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

#ifndef GUARD_report_panel_hpp_8629163596140763
#define GUARD_report_panel_hpp_8629163596140763

#include "account_type.hpp"
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/id.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/window.h>
#include <vector>

namespace dcm
{

// Begin forward declarations

class Account;
class OrdinaryJournal;
class DcmDatabaseConnection;

namespace gui
{

class ComboBox;
class DateCtrl;
class Report;

// End forward declarations

/**
 * Panel for holding date-filtered balance sheet and profit-and-loss
 * reports.
 */
class ReportPanel: public wxPanel
{
public:
	ReportPanel
	(	wxWindow* p_parent,
		DcmDatabaseConnection& p_database_connection
	);

	ReportPanel(ReportPanel const&) = delete;
	ReportPanel(ReportPanel&&) = delete;
	ReportPanel& operator=(ReportPanel const&) = delete;
	ReportPanel& operator=(ReportPanel&&) = delete;
	virtual ~ReportPanel();

	void update_for_new(sqloxx::Handle<OrdinaryJournal> const& p_journal);
	void update_for_amended(sqloxx::Handle<OrdinaryJournal> const& p_journal);
	void update_for_new(sqloxx::Handle<Account> const& p_account);
	void update_for_amended(sqloxx::Handle<Account> const& p_account);
	void update_for_amended_budget(sqloxx::Handle<Account> const& p_account);
	void update_for_deleted(std::vector<sqloxx::Id> const& p_doomed_ids);

private:
	void on_run_button_click(wxCommandEvent& event);

	void configure_top();
	void configure_bottom();

	AccountSuperType selected_account_super_type() const;

	static int const s_min_date_ctrl_id = wxID_HIGHEST + 1;
	static int const s_max_date_ctrl_id = s_min_date_ctrl_id + 1;
	static int const s_run_button_id = s_max_date_ctrl_id + 1;

	int m_next_row;
	
	int m_client_size_aux;
	int m_text_ctrl_height;

	wxGridBagSizer* m_top_sizer;
	ComboBox* m_report_type_ctrl;
	DateCtrl* m_min_date_ctrl;
	DateCtrl* m_max_date_ctrl;
	wxButton* m_run_button;
	Report* m_report;
	DcmDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class ReportPanel

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_report_panel_hpp_8629163596140763
