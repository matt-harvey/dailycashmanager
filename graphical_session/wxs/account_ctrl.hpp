// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_ctrl_hpp
#define GUARD_account_ctrl_hpp

#include "account.hpp"
#include "b_string.hpp"
#include "string_set_validator.hpp"
#include <boost/noncopyable.hpp>
#include <jewel/debug_log.hpp>
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
class AccountCtrl: public wxComboBox, private boost::noncopyable
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
		AccountIter const& p_end
	);
	
	/**
	 * @returns currently selected Account.
	 *
	 * Unfortunately this can't currently be const because the
	 * wxWindow::GetValidator() function used in the implementation, is
	 * not const.
	 */
	Account	account();

	/**
	 * Reset the selections available in the Combobox, to the
	 * Accounts in the range p_beg to p_end. Must not be an
	 * empty range.
	 */
	template <typename AccountIter>
	void set
	(	AccountIter p_beg,
		AccountIter const& p_end
	);

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
	AccountIter const& p_end
):
	wxComboBox
	(	p_parent,
		p_id,
		bstring_to_wx(p_account.name()),
		wxDefaultPosition,
		p_size,
		wxArrayString(),	
		wxCB_SORT
	),
	m_database_connection(p_account.database_connection())
{
	wxArrayString valid_account_names;
	assert (valid_account_names.IsEmpty());
	for ( ; p_beg != p_end; ++p_beg)
	{
		wxString const name_wx = bstring_to_wx(p_beg->name());
		valid_account_names.Add(name_wx);  // remembers as valid name
		Append(name_wx);  // adds to combobox
	}
	StringSetValidator validator
	(	bstring_to_wx(p_account.name()),
		valid_account_names,
		"account or category"
	);
	SetValidator(validator);
	AutoComplete(valid_account_names);
}

template <typename AccountIter>
void
AccountCtrl::set(AccountIter p_beg, AccountIter const& p_end)
{
	assert (p_end > p_beg);
	wxArrayString valid_account_names;
	for ( ; p_beg != p_end; ++p_beg)
	{
		wxString const name_wx = bstring_to_wx(p_beg->name());
		valid_account_names.Add(name_wx);
	}
	assert (!valid_account_names.IsEmpty());
	StringSetValidator validator
	(	valid_account_names[0],
		valid_account_names,
		"account or category"
	);
	Set(valid_account_names);
	SetValidator(validator);
	AutoComplete(valid_account_names);
	SetValue(valid_account_names[0]);
	return;
}


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_ctrl_hpp
