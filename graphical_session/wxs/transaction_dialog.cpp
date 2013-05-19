#include "transaction_dialog.hpp"
#include "account.hpp"
#include <wx/dialog.h>
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
	)
{
	// TODO Implement
}

}  // namespace gui
}  // namespace phatbooks
