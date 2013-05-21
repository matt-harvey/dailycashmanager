#ifndef GUARD_transaction_dialog_hpp
#define GUARD_transaction_dialog_hpp

#include "account.hpp"
#include <wx/dialog.h>
#include <wx/sizer.h>
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
	wxBoxSizer* m_top_sizer;
	static unsigned int const s_start_entry_amount_ids = wxID_HIGHEST + 1;

};  // class TransactionDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_dialog_hpp
