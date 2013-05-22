// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_transaction_dialog_hpp
#define GUARD_transaction_dialog_hpp

#include "account.hpp"
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <vector>

namespace phatbooks
{
namespace gui
{

/**
 * Dialog in which user creates a new transaction (i.e. Journal).
 */
class TransactionDialog: public wxDialog
{
public:
	TransactionDialog(std::vector<Account> const& p_accounts);

protected:

private:
	void on_ok_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	wxFlexGridSizer* m_top_sizer;
	wxTextCtrl* m_date_ctrl;
	wxButton* m_ok_button;
	wxButton* m_cancel_button;

	static unsigned int const s_date_ctrl_id = wxID_HIGHEST + 1;

	DECLARE_EVENT_TABLE()

};  // class TransactionDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_dialog_hpp
