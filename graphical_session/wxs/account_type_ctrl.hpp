// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_type_ctrl_hpp
#define GUARD_account_type_ctrl_hpp

#include "account_type.hpp"
#include <boost/noncopyable.hpp>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/windowid.h>
#include <wx/window.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

// End forward declarations

/**
 * Widget from which user can select an AccountType.
 */
class AccountTypeCtrl: public wxComboBox, private boost::noncopyable
{
public:
	AccountTypeCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		account_super_type::AccountSuperType p_account_super_type
	);

	account_type::AccountType account_type() const;

	/**
	 * @throws InvalidAccountTypeException if p_account_type
	 * does not have m_account_super_type as it AccountSuperType.
	 */
	void set_account_type(account_type::AccountType p_account_type);

private:

	account_super_type::AccountSuperType m_account_super_type;
	PhatbooksDatabaseConnection& m_database_connection;

};  // class AccountTypeCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_type_ctrl_hpp
