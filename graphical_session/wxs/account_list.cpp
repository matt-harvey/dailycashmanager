#include "account_list.hpp"
#include "account_reader.hpp"
#include "b_string.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include <vector>
#include <wx/listctrl.h>
#include <wx/string.h>

using std::vector;


namespace phatbooks
{
namespace gui
{


namespace
{

	AccountList*
	create_account_list_from_reader
	(	wxWindow* parent,
		AccountReaderBase const& reader
	)
	{
		AccountList* ret = new wxListCtrl
		(	parent,
			wxID_ANY,
			wxDefaultPosition,
			wxSize(parent->GetClientSize().GetX() / 4, parent->GetClientSize().GetY()),
			wxLC_REPORT | wxBORDER_SIMPLE | wxFULL_REPAINT_ON_RESIZE
		);

		// Insert name column
		ret->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT);

		// Insert balance column
		ret->InsertColumn(1, "Balance", wxLIST_FORMAT_RIGHT);

		AccountReader::size_type i = 0;
		for
		(	AccountReader::const_iterator it = reader.begin(),
				end = reader.end();
			it != end;
			++it, ++i
		)
		{
			// Insert item, with string for Column 0
			ret->InsertItem(i, bstring_to_wx(it->name()));
		
			// The item may change position due to e.g. sorting, so store the
			// original index in the item's data
			ret->SetItemData(i, i);

			// Insert the balance string
			ret->SetItem
			(	i,
				1,
				bstring_to_wx(finformat_bstring(it->friendly_balance()))
			);
		}
		ret->SetColumnWidth(0, wxLIST_AUTOSIZE);
		ret->SetColumnWidth(1, wxLIST_AUTOSIZE);

		return ret;
	}

}  // end anonymous namespace


AccountList* create_balance_sheet_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	BalanceSheetAccountReader reader(dbc);
	return create_account_list_from_reader(parent, reader);
}


AccountList* create_pl_account_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	PLAccountReader reader(dbc);
	return create_account_list_from_reader(parent, reader);
}




}  // namespace gui
}  // namespace phatbooks
