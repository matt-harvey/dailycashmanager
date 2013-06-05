#ifndef GUARD_account_ctrl_hpp
#define GUARD_account_ctrl_hpp

#include "account.hpp"
#include "account_name_validator.hpp"
#include "b_string.hpp"
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/window.h>
#include <cassert>

namespace phatbooks
{

class PhatbooksDatabaseConnection;

namespace gui
{

/**
 * Widget by means of which the user is enabled to select an
 * \e existing Account.
 */
class AccountCtrl: public wxComboBox
{
public:

	/**
	 * @param p_parent parent window.
	 *
	 * @param p_id id.
	 *
	 * @param p_account Account initially represented in the control.
	 * This Account should be contained within the range p_beg, p_end.
	 * 
	 * @param p_size size.
	 *
	 * @param p_beg Iterator to beginning of sequence of Accounts that will
	 * be considered valid selections for this control.
	 *
	 * @param p_end Iterator to one-past-end of sequence of the Accounts
	 * that will be considered valid selections for this control.
	 */
	template <typename AccountIter>
	AccountCtrl
	(	wxWindow* p_parent,
		unsigned int p_id,
		Account const& p_account,
		wxSize const& p_size,
		AccountIter p_beg,
		AccountIter const& p_end,
		PhatbooksDatabaseConnection& p_database_connection
	);
	
	/**
	 * @returns currently selected Account.
	 *
	 * Unfortunately this can't currently be const because the
	 * wxWindow::GetValidator() function used in the implementation, is
	 * not const.
	 */
	Account	account();

private:
	void on_kill_focus(wxFocusEvent& event);
	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class AccountCtrl


// Implement member function templates

template <typename AccountIter>
AccountCtrl::AccountCtrl
(	wxWindow* p_parent,
	unsigned int p_id,
	Account const& p_account,
	wxSize const& p_size,
	AccountIter p_beg,
	AccountIter const& p_end,
	PhatbooksDatabaseConnection& p_database_connection
):
	m_database_connection(p_database_connection)
{
	wxArrayString valid_account_names;
	assert (valid_account_names.IsEmpty());
	for ( ; p_beg != p_end; ++p_beg)
	{
		valid_account_names.Add(bstring_to_wx(p_beg->name()));
	}
	wxString const account_name_wx = bstring_to_wx(p_account.name());
	Create
	(	p_parent,
		p_id,
		account_name_wx,
		wxDefaultPosition,
		p_size,
		valid_account_names,	
		wxCB_SORT,
		AccountNameValidator(account_name_wx, valid_account_names)
	);
	AutoComplete(valid_account_names);
}


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_ctrl_hpp
