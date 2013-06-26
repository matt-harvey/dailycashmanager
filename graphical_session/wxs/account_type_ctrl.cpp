#include "account_type_ctrl.hpp"
#include "account_dialog.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "phatbooks_exceptions.hpp"
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
	PhatbooksDatabaseConnection& p_database_connection,
	account_super_type::AccountSuperType p_account_super_type
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
	m_account_super_type(p_account_super_type),
	m_database_connection(p_database_connection)
{
	typedef vector<account_type::AccountType> ATypeVec;
	ATypeVec const& atypes = account_types(m_account_super_type);
	ATypeVec::const_iterator it = atypes.begin();
	ATypeVec::const_iterator const end = atypes.end();
	for ( ; it != end; ++it)
	{
		Append(bstring_to_wx(account_type_to_string(*it)));
	}	
	SetSelection(0);  // In effort to avoid apparent bug in Windows
}

account_type::AccountType
AccountTypeCtrl::account_type() const
{
	assert (!GetValue().IsEmpty());
	return string_to_account_type(wx_to_bstring(GetValue()));
}

void
AccountTypeCtrl::set_account_type(account_type::AccountType p_account_type)
{
	if (super_type(p_account_type) != m_account_super_type)
	{
		throw InvalidAccountTypeException
		(	"AccountType is invalid given the AccountSuperType of this "
			"AccountTypeCtrl."
		);
	}
	assert (m_account_super_type == super_type(p_account_type));
	SetValue(bstring_to_wx(account_type_to_string(p_account_type)));
	return;
}


}  // namespace gui
}  // namespace phatbooks
