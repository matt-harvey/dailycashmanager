// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_transaction_ctrl_hpp
#define GUARD_transaction_ctrl_hpp

#include "account.hpp"
#include "decimal_text_ctrl.hpp"
#include <jewel/on_windows.hpp>
#include <wx/button.h>

#ifndef JEWEL_ON_WINDOWS
#	include <wx/calctrl.h>
#else
#	include <wx/datectrl.h>
#endif

#include <wx/event.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class TopPanel;

// End forward declarations




/**
 * Dialog in which user creates a new transaction (i.e. Journal).
 *
 * @todo Ensure Journal balances.
 *
 * @todo Provide option to save as Draft, Recurring or etc. etc..
 */
class TransactionCtrl: public wxPanel
{
public:
	
	/**
	 * All Accounts in p_accounts should be associated with the same
	 * PhatbooksDatabaseConnection.
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		std::vector<Account> const& p_accounts
	);

protected:

private:
	void on_ok_button_click(wxCommandEvent& event);
	void post_journal() const;
	bool is_balanced() const;

	int m_max_entry_row_id;
	wxFlexGridSizer* m_top_sizer;
	wxRadioBox* m_actual_vs_budget_ctrl;

	// Avoid using wxDatePickerCtrl unless on Windows - it's buggy
	// on wxGTK (and I haven't yet tested it on OSX).
#	ifndef JEWEL_ON_WINDOWS
		wxCalendarCtrl*
#	else
		wxDatePickerCtrl*
#	endif
			m_date_ctrl;
	
	wxButton* m_cancel_button;
	wxButton* m_ok_button;

	std::vector<wxStaticText*> m_account_name_boxes;
	std::vector<wxTextCtrl*> m_comment_boxes;
	std::vector<DecimalTextCtrl*> m_amount_boxes;

	static unsigned int const s_date_ctrl_id = wxID_HIGHEST + 1;
	static unsigned int const s_min_entry_row_id = s_date_ctrl_id + 1;

	DECLARE_EVENT_TABLE()

	// Non-owning pointer
	PhatbooksDatabaseConnection* m_database_connection;

};  // class TransactionCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_ctrl_hpp
