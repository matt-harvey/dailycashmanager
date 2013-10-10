/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


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
