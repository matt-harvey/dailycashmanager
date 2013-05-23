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
 *
 * @todo Ensure Journal balances.
 *
 * @todo Provide option to save as Draft, Recurring or etc. etc..
 */
class TransactionDialog: public wxDialog
{
public:
	TransactionDialog(std::vector<Account> const& p_accounts);

protected:

private:
	void on_ok_button_click(wxCommandEvent& event);
	bool is_balanced() const;

	int m_max_entry_row_id;
	wxFlexGridSizer* m_top_sizer;
	wxTextCtrl* m_date_ctrl;
	wxButton* m_ok_button;
	wxButton* m_cancel_button;

	std::vector<wxTextCtrl*> m_amount_boxes;

	static unsigned int const s_date_ctrl_id = wxID_HIGHEST + 1;
	static unsigned int const s_min_entry_row_id = s_date_ctrl_id + 1;

	DECLARE_EVENT_TABLE()

};  // class TransactionDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_dialog_hpp
