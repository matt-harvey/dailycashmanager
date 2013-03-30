#ifndef GUARD_top_panel_hpp
#define GUARD_top_panel_hpp

#include "entry_list_ctrl.hpp"

#include <wx/wx.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class AccountListCtrl;
class Frame;

// End forward declarations


/**
 * Top level panel intended as immediate child of Frame.
 */
class TopPanel:
	public wxPanel
{
public:

	TopPanel
	(	Frame* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);


private:

	PhatbooksDatabaseConnection& m_database_connection;
	wxBoxSizer* m_top_sizer;
	AccountListCtrl* m_pl_account_list;
	EntryListCtrl* m_entry_list;
	AccountListCtrl* m_bs_account_list;
};



}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_top_panel_hpp
