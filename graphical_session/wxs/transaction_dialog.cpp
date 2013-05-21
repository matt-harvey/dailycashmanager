#include "transaction_dialog.hpp"
#include "account.hpp"
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <vector>

using std::vector;

namespace phatbooks
{
namespace gui
{

TransactionDialog::TransactionDialog(vector<Account> const& p_accounts):
	wxDialog
	(	0,  // Null parent implies top-level window is parent
		wxID_ANY,
		"New transaction",
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE
	),
	m_top_sizer(0)
{
	m_top_sizer = new wxBoxSizer(wxVERTICAL);
	vector<Account>::size_type const sz = p_accounts.size();
	vector<Account>::size_type i = 0;
	for ( ; i != sz; ++i)
	{
		wxBoxSizer* row_sizer = new wxBoxSizer(wxHORIZONTAL);
		wxStaticText* account_name_text = new wxStaticText
		(	this,
			wxID_ANY,
			bstring_to_wx(p_accounts[i].name()),
			wxDefaultPosition,
			wxDefaultSize,
			wxALIGN_LEFT
		);
		wxTextCtrl* entry_ctrl = new wxTextCtrl
		(	this,
			s_start_entry_amount_ids + i,
			wxEmptyString,
			wxDefaultPosition,
			wxDefaultSize,
			0  // style
			// TODO Need a validator
		);
		m_top_sizer->Add(row_sizer);
		row_sizer->Add(account_name_text, wxSizerFlags(1).Expand());
		row_sizer->Add(entry_ctrl, 0, wxLEFT, 5);
	}
	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	Layout();
}


}  // namespace gui
}  // namespace phatbooks
