#include "entry_list_panel.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/panel.h>
#include <wx/window.h>

using boost::optional;
using jewel::value;

namespace phatbooks
{
namespace gui
{

EntryListPanel::EntryListPanel
(	wxWindow* p_parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel(p_parent, wxID_ANY),
	m_account_ctrl(0),
	m_min_date_selector(0),
	m_max_date_selector(0),
	m_entry_list_ctrl(0),
	m_database_connection(p_database_connection)
{
	// TODO HIGH PRIORITY Finish implementing this.
}

optional<Account>
EntryListPanel::maybe_account() const
{
	optional<Account> ret;
	// TODO We need a mechanism for returning an unitialized optional - which
	// would signify that we will show Entries of any Account.
	assert (m_account_ctrl);
	ret = m_account_ctrl->account();
	return ret;
}

}  // namespace gui
}  // namespace phatbooks
