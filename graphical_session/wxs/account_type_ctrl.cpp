#include "account_type_ctrl.hpp"
#include "account_dialog.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include <wx/combobox.h>
#include <vector>

using std::vector;

namespace phatbooks
{
namespace gui
{

AccountTypeCtrl::AccountTypeCtrl
(	AccountDialog* p_parent,
	wxWindowID p_id,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxComboBox
	(	p_parent,
		p_id,
		wxEmptyString,  // TODO HIGH PRIORITY Needs to be an AccountType name
		wxDefaultPosition,
		p_size,
		wxArrayString(),
		wxCB_READONLY
	),
	m_database_connection(p_database_connection)
{
	vector<BString>::const_iterator it = account_type_names().begin();
	vector<BString>::const_iterator const end = account_type_names().end();
	for ( ; it != end; ++it) Append(bstring_to_wx(*it));
	SetSelection(0);  // In effort to avoid apparent bug in Windows
}

account_type::AccountType
AccountTypeCtrl::account_type() const
{
	assert (!GetValue().IsEmpty());
	return string_to_account_type(wx_to_bstring(GetValue()));
}


}  // namespace gui
}  // namespace phatbooks
