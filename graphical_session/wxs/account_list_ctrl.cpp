#include "account_list_ctrl.hpp"
#include "account_reader.hpp"
#include "b_string.hpp"
#include "finformat.hpp"
#include "app.hpp"
#include "phatbooks_database_connection.hpp"
#include <vector>
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/wx.h>

using std::vector;


namespace phatbooks
{
namespace gui
{


AccountListCtrl*
AccountListCtrl::create_balance_sheet_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	BalanceSheetAccountReader const reader(dbc);
	AccountListCtrl* ret = new AccountListCtrl(parent, reader, dbc);
	return ret;
}

AccountListCtrl*
AccountListCtrl::create_pl_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	PLAccountReader const reader(dbc);
	AccountListCtrl* ret = new AccountListCtrl(parent, reader, dbc, true);
	return ret;
}

AccountListCtrl::AccountListCtrl
(	wxWindow* p_parent,
	AccountReaderBase const& p_reader,
	PhatbooksDatabaseConnection& p_database_connection,
	bool p_show_daily_budget
):
	wxListCtrl
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		wxSize
		(	p_parent->GetClientSize().GetX() / 4,
			p_parent->GetClientSize().GetY()
		),
		wxLC_REPORT | wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection)
{
	InsertColumn(0, "Name", wxLIST_FORMAT_LEFT);
	InsertColumn(1, "Balance", wxLIST_FORMAT_RIGHT);
	if (p_show_daily_budget)
	{
		InsertColumn(2, "Daily budget", wxLIST_FORMAT_RIGHT);
	}

	AccountReader::size_type i = 0;

	App* app = dynamic_cast<App*>(wxTheApp);
	wxLocale const& locale = app->locale();

	for
	(	AccountReader::const_iterator it = p_reader.begin(),
			end = p_reader.end();
		it != end;
		++it, ++i
	)
	{
		// Insert item, with string for Column 0
		InsertItem(i, bstring_to_wx(it->name()));
	
		// The item may change position due to e.g. sorting, so store the
		// original index in the item's data
		SetItemData(i, i);

		// Insert the balance string
		SetItem(i, 1, finformat_wx(it->friendly_balance(), locale));

		if (p_show_daily_budget)
		{
			// Insert budget string
			SetItem(i, 2, finformat_wx(it->budget(), locale));
		}
	}
	SetColumnWidth(0, wxLIST_AUTOSIZE);
	SetColumnWidth(1, wxLIST_AUTOSIZE);
	if (p_show_daily_budget)
	{
		SetColumnWidth(2, wxLIST_AUTOSIZE);
	}
}



}  // namespace gui
}  // namespace phatbooks
