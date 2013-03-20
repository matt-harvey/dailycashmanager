#include "entry_list.hpp"
#include "account.hpp"
#include "b_string.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "entry_reader.hpp"
#include "my_app.hpp"
#include "ordinary_journal.hpp"
#include <boost/lexical_cast.hpp>
#include <string>

using boost::lexical_cast;
using std::string;

namespace phatbooks
{
namespace gui
{

namespace
{

	EntryList*
	create_ordinary_entry_list_from_reader
	(	wxWindow* parent,
		EntryReader const& reader
	)
	{
		static const int date_col_num = 0;
		static const int account_col_num = 1;
		static const int comment_col_num = 2;
		static const int amount_col_num = 3;
		static const int reconciled_col_num = 4;
		static const int num_columns = 5;
		
		EntryList* ret = new wxListCtrl
		(	parent,
			wxID_ANY,
			wxDefaultPosition,
			wxSize(parent->GetClientSize().GetX() / 2, parent->GetClientSize().GetY()),
			wxLC_REPORT | wxFULL_REPAINT_ON_RESIZE
		);

		// Insert columns
		ret->InsertColumn(date_col_num, "Date", wxLIST_FORMAT_RIGHT);
		ret->InsertColumn(account_col_num, "Account", wxLIST_FORMAT_LEFT);
		ret->InsertColumn(comment_col_num, "Comment", wxLIST_FORMAT_LEFT);
		ret->InsertColumn(amount_col_num, "Amount", wxLIST_FORMAT_RIGHT);
		ret->InsertColumn(reconciled_col_num, "R", wxLIST_FORMAT_LEFT);

		// WARNING This sucks
		MyApp* app = dynamic_cast<MyApp*>(wxTheApp);

		EntryReader::size_type i = 0;
		for
		(	EntryReader::const_iterator it = reader.begin(),
				end = reader.end();
			it != end;
			++it, ++i
		)
		{
			OrdinaryJournal journal(it->journal<OrdinaryJournal>());

			wxString const wx_date_string = date_format_wx(journal.date());

			wxString const account_string = bstring_to_wx
			(	it->account().name()
			);
			wxString const comment_string = bstring_to_wx(it->comment());

			// WARNING This sucks
			wxString const amount_string = finformat_wx(it->amount(), app->locale());

			// TODO Should have a tick icon here rather than a "Y".
			wxString const reconciled_string =
				(it->is_reconciled()? "Y": "N");
	
			// Populate 0th column
			assert (date_col_num == 0);
			ret->InsertItem(i, wx_date_string);

			// The item may change position due to e.g. sorting, so store the
			// original index in the item's data
			ret->SetItemData(i, i);
		
			// Populate the other columns
			ret->SetItem(i, account_col_num, account_string);
			ret->SetItem(i, comment_col_num, comment_string);
			ret->SetItem(i, amount_col_num, amount_string);
			ret->SetItem(i, reconciled_col_num, reconciled_string);
		}
		for (int j = 0; j != 5; ++j)
		{
			ret->SetColumnWidth(j, wxLIST_AUTOSIZE);
		}
		ret->SetColumnWidth(comment_col_num, ret->GetColumnWidth(account_col_num));
		return ret;
	}

}  // end anonymous namespace


EntryList* create_actual_ordinary_entry_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	ActualOrdinaryEntryReader const reader(dbc);
	return create_ordinary_entry_list_from_reader(parent, reader);
}


}  // namespace gui
}  // namespace phatbooks
