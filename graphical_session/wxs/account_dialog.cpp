#include "account_dialog.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "decimal_text_ctrl.hpp"
#include "frame.hpp"
#include <boost/noncopyable.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

AccountDialog::AccountDialog
(	Frame* p_parent,
	Account& p_account,
	account_super_type::AccountSuperType p_account_super_type
):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_top_sizer(0),
	m_name_ctrl(0),
	m_account_type_ctrl(0),
	m_description_ctrl(0),
	m_opening_amount_ctrl(0),
	m_account(p_account)
{
	m_top_sizer = new wxGridBagSizer;
	SetSizer(m_top_sizer);





	// TODO Implement
}

}  // namespace gui
}  // namespace phatbooks
