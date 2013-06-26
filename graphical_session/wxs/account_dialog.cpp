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
#include <cassert>

namespace phatbooks
{
namespace gui
{

namespace
{
	wxString account_super_type_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		switch (p_account_super_type)
		{
		case account_super_type::balance_sheet:
			return wxString("Account");
		case account_super_type::pl:
			return wxString("Category");
		default:
			assert (false);
		}
		assert (false);
	}

	wxString account_name_ctrl_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" name:");
	}

	wxString account_type_ctrl_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" type:");
	}

	wxString account_description_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		(void)p_account_super_type;  // silence compiler re. unused param.
		return wxString("Description:");
	}

	wxString opening_amount_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		switch (p_account_super_type)
		{
		case account_super_type::balance_sheet:
			return wxString("Opening balance:");
		case account_super_type::pl:
			return wxString("Initial budget allocation:");
		default:
			assert (false);
		}
		assert (false);
	}
	
}  // end anonymous namespace


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
