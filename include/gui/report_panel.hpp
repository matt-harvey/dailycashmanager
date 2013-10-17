/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_report_panel_hpp_8629163596140763
#define GUARD_report_panel_hpp_8629163596140763

#include "account_type.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle_fwd.hpp>
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
