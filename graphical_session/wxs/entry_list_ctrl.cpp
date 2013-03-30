#include "entry_list_ctrl.hpp"
#include "account.hpp"
#include "app.hpp"
#include "b_string.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "entry_reader.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/lexical_cast.hpp>
#include <string>

using boost::lexical_cast;
using std::string;

namespace phatbooks
{
namespace gui
{

EntryListCtrl*
EntryListCtrl::create_actual_ordinary_entry_list
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	ActualOrdinaryEntryReader const reader(dbc);
	EntryListCtrl* ret = new EntryListCtrl(parent, reader, dbc);
	return ret;
}


EntryListCtrl::EntryListCtrl
(	wxWindow* p_parent,
	EntryReader const& p_reader,
	PhatbooksDatabaseConnection& p_database_connection	
):
	wxListCtrl
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		wxSize
		(	p_parent->GetClientSize().GetX() / 2,
			p_parent->GetClientSize().GetY()
		),
		wxLC_REPORT | wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection)
{
	static const int date_col_num = 0;
	static const int account_col_num = 1;
	static const int comment_col_num = 2;
	static const int amount_col_num = 3;
	static const int reconciled_col_num = 4;
	static const int num_columns = 5;

	// Insert columns
	InsertColumn(date_col_num, "Date", wxLIST_FORMAT_RIGHT);
	InsertColumn(account_col_num, "Account", wxLIST_FORMAT_LEFT);
	InsertColumn(comment_col_num, "Comment", wxLIST_FORMAT_LEFT);
	InsertColumn(amount_col_num, "Amount", wxLIST_FORMAT_RIGHT);
	InsertColumn(reconciled_col_num, "R", wxLIST_FORMAT_LEFT);

	// WARNING This sucks
	App* app = dynamic_cast<App*>(wxTheApp);

	EntryReader::size_type i = 0;
	for
	(	EntryReader::const_iterator it = p_reader.begin(),
			end = p_reader.end();
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
		wxString const amount_string =
			finformat_wx(it->amount(), app->locale());

		// TODO Should have a tick icon here rather than a "Y".
		wxString const reconciled_string =
			(it->is_reconciled()? "Y": "N");

		// Populate 0th column
		assert (date_col_num == 0);
		InsertItem(i, wx_date_string);

		// The item may change position due to e.g. sorting, so store the
		// original index in the item's data
		SetItemData(i, i);
	
		// Populate the other columns
		SetItem(i, account_col_num, account_string);
		SetItem(i, comment_col_num, comment_string);
		SetItem(i, amount_col_num, amount_string);
		SetItem(i, reconciled_col_num, reconciled_string);
	}
	for (int j = 0; j != 5; ++j)
	{
		SetColumnWidth(j, wxLIST_AUTOSIZE);
	}
	SetColumnWidth
	(	comment_col_num,
		GetColumnWidth(account_col_num)
	);
}



}  // namespace gui
}  // namespace phatbooks
