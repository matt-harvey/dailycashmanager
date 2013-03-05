#include "account_list.hpp"
#include "account_reader.hpp"
#include "b_string.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/listctrl.h>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{




AccountList*
create_account_list(wxWindow* parent, PhatbooksDatabaseConnection& dbc)
{
	AccountList* ret = new wxListCtrl
	(	parent,
		wxID_ANY,
		wxDefaultPosition,
		wxSize(parent->GetSize().GetX() / 3, parent->GetSize().GetY()),
		wxLC_REPORT
	);

	// Insert name column
	wxListItem name_column;
	name_column.SetText("Name");
	ret->InsertColumn(0, name_column);
	ret->SetColumnWidth(0, wxLIST_AUTOSIZE);

	// Insert balance column
	wxListItem balance_column;
	balance_column.SetText("Balance");
	ret->InsertColumn(1, balance_column);
	ret->SetColumnWidth(1, wxLIST_AUTOSIZE);

	BalanceSheetAccountReader bs_reader(dbc);
	PLAccountReader pl_reader(dbc);
	BalanceSheetAccountReader::size_type bi = 0;
	for
	(	BalanceSheetAccountReader::const_iterator blit = bs_reader.begin();
		blit != bs_reader.end();
		++blit, ++bi
	)
	{
		ret->InsertItem(bi, bstring_to_wx(blit->name()));
	}


	return ret;
}






}  // namespace gui
}  // namespace phatbooks
