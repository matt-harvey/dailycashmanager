#ifndef GUARD_account_list_ctrl_hpp
#define GUARD_account_list_ctrl_hpp

#include <wx/listctrl.h>

namespace phatbooks
{

class PhatbooksDatabaseConnection;


namespace gui
{


typedef wxListCtrl AccountListCtrl;

/**
 * @returns a pointer to a heap-allocated AccountListCtrl, listing
 * all and only the balance sheet accounts stored in \e dbc.
 * The client does not need to take care of the memory - the memory
 * is taken care of by the parent window.
 */
AccountListCtrl* create_balance_sheet_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
);
	
/**
 * @returns a pointer to a heap-allocated AccountListCtrl, listing
 * all and only the profit-and-loss accounts stored in \e dbc.
 * The client does not need to take care of the memory - the memory
 * is taken care of by the parent window.
 */
AccountListCtrl* create_pl_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
);
	

}  // namespace gui
}  // namespace phatbooks



#endif  // GUARD_account_list_ctrl_hpp
