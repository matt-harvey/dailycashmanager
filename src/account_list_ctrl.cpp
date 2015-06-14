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

#include "gui/account_list_ctrl.hpp"
#include "app.hpp"
#include "account.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "finformat.hpp"
#include "dcm_database_connection.hpp"
#include "string_flags.hpp"
#include "gui/account_dialog.hpp"
#include "gui/locale.hpp"
#include "gui/persistent_object_event.hpp"
#include "gui/top_panel.hpp"
#include <jewel/assert.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/string.h>
#include <algorithm>
#include <set>

using sqloxx::Handle;
using std::is_signed;
using std::max;
using std::set;

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(AccountListCtrl, wxListCtrl)
    EVT_LIST_ITEM_ACTIVATED
    (   wxID_ANY,    
        AccountListCtrl::on_item_activated
    )
END_EVENT_TABLE()

AccountListCtrl::AccountListCtrl
(   wxWindow* p_parent,
    DcmDatabaseConnection& p_database_connection,
    AccountSuperType p_account_super_type
):
    wxListCtrl
    (   p_parent,
        wxID_ANY,
        wxDefaultPosition,
        wxSize
        (   p_parent->GetClientSize().GetX() / 4,
            p_parent->GetClientSize().GetY()
        ),
        wxLC_REPORT | wxLC_SINGLE_SEL | wxFULL_REPAINT_ON_RESIZE
    ),
    m_show_hidden(false),
    m_account_super_type(p_account_super_type),
    m_database_connection(p_database_connection)
{
    update();
}

AccountListCtrl::~AccountListCtrl()
{
}

set<sqloxx::Id>
AccountListCtrl::selected_accounts() const
{
    set<sqloxx::Id> ret;
    size_t i = 0;
    size_t const lim = GetItemCount();
    for ( ; i != lim; ++i)
    {
        if (GetItemState(i, wxLIST_STATE_SELECTED))
        {
            Handle<Account> const account
            (   m_database_connection,
                GetItemData(i)
            );
            JEWEL_ASSERT (account->has_id());
            ret.insert(account->id());
        }
    }
    return ret;
}    

void
AccountListCtrl::on_item_activated(wxListEvent& event)
{
    JEWEL_LOG_TRACE();
    sqloxx::Id const account_id = GetItemData(event.GetIndex());

    // Fire an Account editing request. This will be handled higher up
    // the window hierarchy.
    PersistentObjectEvent::fire
    (   this,
        DCM_ACCOUNT_EDITING_EVENT,
        account_id
    );
    JEWEL_LOG_TRACE();
    return;
 }

void
AccountListCtrl::update()
{
    // Remember which rows are selected currently
    auto const selected = selected_accounts();

    // Remember scrolled position
    auto const item_count = GetItemCount();
    auto const top_item = GetTopItem();
    auto const last_item = item_count - 1;
    auto const last_position = top_item + GetCountPerPage();
    long bottom_item;
    if (last_position < last_item)
    {
        bottom_item = last_position - 1; 
    }
    else if (last_position == last_item)
    {
        bottom_item = last_item - 1;
    }
    else
    {
        bottom_item = last_item;
    }

    // Now (re)draw
    ClearAll();
    InsertColumn
    (   s_name_col,
        account_concept_name
        (   m_account_super_type,
            AccountPhraseFlags().set(string_flags::capitalize)
        ),
        wxLIST_FORMAT_LEFT
    );
    InsertColumn
    (   s_balance_col,
        wxString("Balance"),
        wxLIST_FORMAT_RIGHT
    );
    if (showing_daily_budget())
    {
        InsertColumn(s_budget_col, "Daily top-up", wxLIST_FORMAT_RIGHT);
    }

    long i = 0;  // because wxWidgets uses long
    AccountTableIterator it = make_type_name_ordered_account_table_iterator
    (   m_database_connection
    );
    AccountTableIterator const end;
    for ( ; it != end; ++it)
    {
        Handle<Account> const& account = *it;
        if
        (   (account->account_super_type() == m_account_super_type) &&
            (m_show_hidden || (account->visibility() == Visibility::visible))
        )    
        {
            // Insert item, with string for Column 0
            InsertItem(i, account->name());
        
            JEWEL_ASSERT (account->has_id());
            auto const id = account->id();
            static_assert
            (   (sizeof(id) <= sizeof(long)) &&
                is_signed<decltype(id)>::value &&
                is_signed<long>::value,
                "Object Id is too wide to be safely passed to "
                "SetItemData."
            );
            SetItemData(i, id);

            // Insert the balance string
            SetItem
            (   i,
                s_balance_col,
                finformat_wx(account->friendly_balance(), locale())
            );

            if (showing_daily_budget())
            {
                // Insert budget string
                SetItem
                (   i,
                    s_budget_col,
                    finformat_wx(account->budget(), locale())
                );
            }

            ++i;
        }
    }

    // Reinstate the selections we remembered
    for (size_t j = 0, lim = GetItemCount(); j != lim; ++j)
    {
        Handle<Account> const account
        (   m_database_connection,
            GetItemData(j)
        );
        JEWEL_ASSERT (account->has_id());
        if (selected.find(account->id()) != selected.end())
        {
            SetItemState
            (   j,
                wxLIST_STATE_SELECTED,
                wxLIST_STATE_SELECTED
            );
        }
    }

    // Configure column widths
    SetColumnWidth(s_name_col, wxLIST_AUTOSIZE_USEHEADER);
    SetColumnWidth(s_name_col, max(GetColumnWidth(s_name_col), 200));
    SetColumnWidth(s_balance_col, wxLIST_AUTOSIZE);
    SetColumnWidth(s_balance_col, max(GetColumnWidth(s_balance_col), 90));
    if (showing_daily_budget())
    {
        SetColumnWidth(s_budget_col, wxLIST_AUTOSIZE);
        SetColumnWidth(s_budget_col, max(GetColumnWidth(s_budget_col), 90));
    }

    // Reinstate scrolled position
    if (bottom_item >= 0) EnsureVisible(bottom_item);

    Layout();

    return;
}

bool
AccountListCtrl::showing_daily_budget() const
{
    return m_account_super_type == AccountSuperType::pl;
}

Handle<Account>
AccountListCtrl::default_account() const
{
    Handle<Account> ret;
    if (GetItemCount() != 0)
    {
        // Return the first Account that is actually showing in the
        // control.
        JEWEL_ASSERT (GetItemCount() > 0);
        ret = Handle<Account>(m_database_connection, GetItemData(GetTopItem()));
    }
    else
    {
        // Return the an Account of the AccountSuperType of this control,
        // even though not showing in the control (because hidden).
        AccountTableIterator it = make_type_name_ordered_account_table_iterator
        (   m_database_connection
        );
        AccountTableIterator const end;
        for ( ; it != end; ++it)
        {
            Handle<Account> const account = *it;
            if (account->account_super_type() == m_account_super_type)
            {
                ret = account;
                break;
            }
        }
    }
    return ret;
}

void
AccountListCtrl::select_only(Handle<Account> const& p_account)
{
    JEWEL_ASSERT (p_account->has_id());  // precondition    

    size_t const sz = GetItemCount();    
    for (size_t i = 0; i != sz; ++i)
    {
        Handle<Account> const account(m_database_connection, GetItemData(i));
        long const filter = (wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
        long const flags = ((account == p_account)? filter: 0);
        SetItemState(i, flags, filter);
    }
    return;
}

bool
AccountListCtrl::toggle_showing_hidden()
{
    m_show_hidden = !m_show_hidden;
    update();
    return m_show_hidden;
}



}  // namespace gui
}  // namespace dcm
