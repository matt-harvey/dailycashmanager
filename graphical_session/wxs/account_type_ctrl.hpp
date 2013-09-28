// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_type_ctrl_hpp_7303785568710317
#define GUARD_account_type_ctrl_hpp_7303785568710317

#include "account_type.hpp"
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
class AccountTypeCtrl: public wxComboBox
{
public:
	AccountTypeCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		AccountSuperType p_account_super_type
	);

	AccountTypeCtrl(AccountTypeCtrl const&) = delete;
	AccountTypeCtrl(AccountTypeCtrl&&) = delete;
	AccountTypeCtrl& operator=(AccountTypeCtrl const&) = delete;
	AccountTypeCtrl& operator=(AccountTypeCtrl&&) = delete;

	~AccountTypeCtrl() = default;

	AccountType account_type() const;

	/**
	 * @throws InvalidAccountTypeException if p_account_type
	 * does not have m_account_super_type as it AccountSuperType.
	 */
	void set_account_type(AccountType p_account_type);

private:

	AccountSuperType m_account_super_type;
	PhatbooksDatabaseConnection& m_database_connection;

};  // class AccountTypeCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_type_ctrl_hpp_7303785568710317
