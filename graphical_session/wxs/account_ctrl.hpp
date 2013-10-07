// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_ctrl_hpp_7150714070140717
#define GUARD_account_ctrl_hpp_7150714070140717

#include "account.hpp"
#include "account_type.hpp"
#include <jewel/assert.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/window.h>
#include <map>
#include <set>
#include <vector>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

// end forward declarations


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
	 * @param p_size size.
	 *
	 * @param p_account_types the AccountTypes we want represented in the
	 * AccountCtrl. This must not be empty.
	 *
	 * @param p_exclude_balancing_account If \e true, then the
	 * sqloxx::Handle<Account> returned by
	 * p_account.database_connection().balancing_account() will not
	 * appear in the Combobox, even if it is in the range provided.
	 *
	 * All Accounts in the given range must have IDs.
	 *
	 * @todo Guard against \e p_account_types being empty.
	 */
	AccountCtrl
	(	wxWindow* p_parent,
		unsigned int p_id,
		wxSize const& p_size,
		std::vector<AccountType> const& p_account_types,
		PhatbooksDatabaseConnection& p_database_connection,
		bool p_exclude_balancing_account = false
	);

	AccountCtrl(AccountCtrl const&) = delete;
	AccountCtrl(AccountCtrl&&) = delete;
	AccountCtrl& operator=(AccountCtrl const&) = delete;
	AccountCtrl& operator=(AccountCtrl&&) = delete;
	virtual ~AccountCtrl();

	/**
	 * Reset the selections available in the Combobox, to all the
	 * Accounts with AccountTypes given by p_account_types.
	 * However, if p_exclude_balancing_account is \e true, then
	 * the sqloxx::Handle<Account> returned by
	 * <em>m_database_connection.balancing_account()</em> will \e not
	 * appear in the Combobox, even if it is in the range provided.
	 *
	 * All Accounts in the range must have Ids.
	 */
	void reset
	(	std::vector<AccountType> const& p_account_types,
		bool p_exclude_balancing_account = false
	);

	/**
	 * @returns sqloxx::Handle to currently selected Account.
	 *
	 * Unfortunately this can't currently be const because the
	 * wxWindow::GetValidator() function used in the implementation, is
	 * not const.
	 */
	sqloxx::Handle<Account> account();

	void set_account(sqloxx::Handle<Account> const& p_account);

	void update_for_new(sqloxx::Handle<Account> const& p_account);
	void update_for_amended(sqloxx::Handle<Account> const& p_account);

private:
	void on_kill_focus(wxFocusEvent& event);

	void reset();

	void refresh();

	bool m_exclude_balancing_account;
	PhatbooksDatabaseConnection& m_database_connection;

	typedef std::map<wxString, sqloxx::Id> AccountMap;
	AccountMap m_account_map;

	std::set<AccountType> m_available_account_types;

	DECLARE_EVENT_TABLE()

};  // class AccountCtrl


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_ctrl_hpp_7150714070140717
