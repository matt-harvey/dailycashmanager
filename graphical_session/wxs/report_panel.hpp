// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_report_panel_hpp_8629163596140763
#define GUARD_report_panel_hpp_8629163596140763

#include "entry.hpp"
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/window.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class OrdinaryJournal;
class PhatbooksDatabaseConnection;

namespace gui
{

class DateCtrl;
class Report;

// End forward declarations

/**
 * Panel for holding date-filtered balance sheet and profit-and-loss
 * reports.
 *
 * @todo We can make things more straightforward and speed start-up times
 * by having the date boxes start out blank. We can then have
 * BalanceSheetReport initially just use the balance() of each Account
 * simpliciter as "Closing balance" and "Movement", and use a
 * \e nil amount for the "Opening balance" column. This can be written
 * into the code for initializing the m_balance_map.
 */
class ReportPanel: public wxPanel
{
public:
	ReportPanel
	(	wxWindow* p_parent,
		PhatbooksDatabaseConnection& p_database_connection
	);

	ReportPanel(ReportPanel const&) = delete;
	ReportPanel(ReportPanel&&) = delete;
	ReportPanel& operator=(ReportPanel const&) = delete;
	ReportPanel& operator=(ReportPanel&&) = delete;
	virtual ~ReportPanel() = default;

	void update_for_new(OrdinaryJournal const& p_journal);
	void update_for_amended(OrdinaryJournal const& p_journal);
	void update_for_new(Account const& p_account);
	void update_for_amended(Account const& p_account);
	void update_for_deleted(std::vector<Entry::Id> const& p_doomed_ids);

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
	wxComboBox* m_report_type_ctrl;
	DateCtrl* m_min_date_ctrl;
	DateCtrl* m_max_date_ctrl;
	wxButton* m_run_button;
	Report* m_report;
	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class ReportPanel

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_report_panel_hpp_8629163596140763
