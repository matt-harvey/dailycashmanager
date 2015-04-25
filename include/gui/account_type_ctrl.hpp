/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_account_type_ctrl_hpp_7303785568710317
#define GUARD_account_type_ctrl_hpp_7303785568710317

#include "account_type.hpp"
#include "gui/combo_box.hpp"
#include <wx/gdicmn.h>
#include <wx/windowid.h>
#include <wx/window.h>

namespace dcm
{

// Begin forward declarations

class DcmDatabaseConnection;

namespace gui
{

// End forward declarations

/**
 * Widget from which user can select an AccountType.
 */
class AccountTypeCtrl: public ComboBox
{
public:
    AccountTypeCtrl
    (   wxWindow* p_parent,
        wxWindowID p_id,
        wxSize const& p_size,
        DcmDatabaseConnection& p_database_connection,
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
    DcmDatabaseConnection& m_database_connection;

};  // class AccountTypeCtrl

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_account_type_ctrl_hpp_7303785568710317
