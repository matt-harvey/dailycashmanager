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

#include "gui/multi_account_panel.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "augmented_account.hpp"
#include "commodity.hpp"
#include "finformat.hpp"
#include "make_default_accounts.hpp"
#include "dcm_database_connection.hpp"
#include "string_flags.hpp"
#include "visibility.hpp"
#include "gui/account_type_ctrl.hpp"
#include "gui/button.hpp"
#include "gui/check_box.hpp"
#include "gui/gridded_scrolled_panel.hpp"
#include "gui/locale.hpp"
#include "gui/opening_balance_ctrl.hpp"
#include "gui/setup_wizard.hpp"
#include "gui/sizing.hpp"
#include "gui/text_ctrl.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/log.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/wupdlock.h>
#include <set>
#include <vector>

using jewel::Decimal;
using jewel::Log;
using jewel::round;
using sqloxx::Handle;
using std::set;
using std::vector;

namespace dcm
{
namespace gui
{

namespace
{
    vector<Handle<Account>> suggested_accounts
    (   DcmDatabaseConnection& p_database_connection,
        AccountSuperType p_account_super_type
    )
    {
        vector<Handle<Account>> ret;
        JEWEL_ASSERT (ret.empty());
        using ATypeVec = vector<AccountType>;
        ATypeVec const& account_types = dcm::account_types(p_account_super_type);
        for (AccountType atype: account_types)
        {
            make_default_accounts(p_database_connection, ret, atype);
        }
        return ret;
    }

}  // end anonymous namespace

MultiAccountPanel::MultiAccountPanel
(   SetupWizard::AccountPage* p_parent,
    wxSize const& p_size,
    DcmDatabaseConnection& p_database_connection,
    AccountSuperType p_account_super_type,
    Handle<Commodity> const& p_commodity,
    size_t p_minimum_num_rows
):
    GriddedScrolledPanel(p_parent, p_size, p_database_connection),
    m_account_super_type(p_account_super_type),
    m_commodity(p_commodity),
    m_summary_amount_text(nullptr),
    m_minimum_num_rows(p_minimum_num_rows)
{
    JEWEL_ASSERT (m_account_names_already_taken.empty());

    // Row of total text etc.
    wxString summary_label("Total");
    if (m_account_super_type == AccountSuperType::pl)
    {
        summary_label += wxString(" to allocate");
    }
    summary_label += ":";
    display_text(summary_label, 3, wxALIGN_RIGHT);
    m_summary_amount_text = display_decimal
    (   summary_amount(),
        4,
        false
    );
    // Dummy column to right
    display_text(wxEmptyString, 5);

    increment_row();
    increment_row();

    // Row of column headings
    AccountPhraseFlags const flags =
        AccountPhraseFlags().set(string_flags::capitalize);
    wxString const account_name_label =
        wxString(" ") +
        account_concept_name(m_account_super_type, flags) +
        wxString(" name:");
    display_text(account_name_label, 0);
    display_text(wxString(" Type:"), 1);
    display_text(wxString(" Description (optional):"), 2);
    // Deliberately skipping column 3.
    switch (m_account_super_type)
    {
    case AccountSuperType::balance_sheet:
        display_text(wxString(" Starting balance:"), 4);
        break;
    case AccountSuperType::pl:
        display_text(wxString(" Initial allocation:"), 4);
        break;
    default:
        JEWEL_HARD_ASSERT (false);
    }

    increment_row();

    // Main body of MultiAccountPanel - a grid of fields where user
    // can edit Account attributes and opening balances.
    vector<Handle<Account>> sugg_accounts =
        suggested_accounts(database_connection(), m_account_super_type);
    auto const sz = sugg_accounts.size();
    m_account_name_boxes.reserve(sz);
    m_account_type_boxes.reserve(sz);
    m_description_boxes.reserve(sz);
    m_opening_balance_boxes.reserve(sz);
    m_check_boxes.reserve(sz);
    for (auto const& account: sugg_accounts)
    {
        push_row(account);
    }
    while (num_rows() < m_minimum_num_rows)
    {
        push_row();
    }
    JEWEL_ASSERT (num_rows() >= p_minimum_num_rows);

    // "Admin"
    FitInside();
    // Layout();
}

MultiAccountPanel::~MultiAccountPanel()
{
}

int
MultiAccountPanel::required_width()
{
    return
        medium_width() * 3 +
        large_width() * 1 +
        standard_gap() * 5 +
        standard_border() * 2 +
        scrollbar_width_allowance();
}

void
MultiAccountPanel::push_row()
{
    Handle<Account> account = blank_account();
    push_row(account);
    return;
}

bool
MultiAccountPanel::remove_checked_rows()
{
    wxWindowUpdateLocker const window_update_locker(this);

    auto const scrolled_position = GetScrollPos(wxVERTICAL);

    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (num_checked_rows() <= m_account_name_boxes.size());
    if (m_account_name_boxes.size() - num_checked_rows() < m_minimum_num_rows)
    {
        JEWEL_LOG_TRACE();
        return false; 
    }
#   ifndef NDEBUG
        vector<TextCtrl*>::size_type const sz = m_account_name_boxes.size();
        JEWEL_ASSERT (sz > 0);
        JEWEL_ASSERT (sz == m_account_type_boxes.size());
        JEWEL_ASSERT (sz == m_description_boxes.size());
        JEWEL_ASSERT (sz == m_opening_balance_boxes.size());
        JEWEL_ASSERT (sz == m_check_boxes.size());
#   endif
    vector<AugmentedAccount> aug_accounts = augmented_accounts();
    JEWEL_ASSERT (aug_accounts.size() == m_check_boxes.size());
    vector<AugmentedAccount> preserved_aug_accounts;
    for (vector<AugmentedAccount>::size_type i = 0; i != aug_accounts.size(); ++i)
    {
        if (!m_check_boxes[i]->GetValue())
        {
            preserved_aug_accounts.push_back(aug_accounts[i]);
        }
    }
    if (preserved_aug_accounts.size() == aug_accounts.size())
    {
        return true;
    }
    auto const lim = m_account_name_boxes.size();
    remove_widgets_from(m_check_boxes);
    remove_widgets_from(m_opening_balance_boxes);
    remove_widgets_from(m_description_boxes);
    remove_widgets_from(m_account_type_boxes);
    remove_widgets_from(m_account_name_boxes);
    increment_row(-lim);
    for (auto const& preserved_aug_account: preserved_aug_accounts)
    {
        push_row(preserved_aug_account.account);
        m_opening_balance_boxes.back()->set_amount
        (    m_account_super_type == AccountSuperType::pl?
            -preserved_aug_account.technical_opening_balance:
            preserved_aug_account.technical_opening_balance
        );
    }
    update_summary();

    Scroll(wxVERTICAL, scrolled_position);  // restore original position

    FitInside();
    JEWEL_LOG_TRACE();
    return true;
}

Decimal
MultiAccountPanel::summary_amount() const
{
    if (m_account_super_type == AccountSuperType::balance_sheet)
    {
        return total_amount();
    }
    JEWEL_ASSERT (m_account_super_type == AccountSuperType::pl);
    SetupWizard::AccountPage const* const parent =
        dynamic_cast<SetupWizard::AccountPage const*>(GetParent());
    JEWEL_ASSERT (parent);
    return parent->total_balance_sheet_amount() - total_amount();
}

void
MultiAccountPanel::update_summary()
{
    // TODO HIGH PRIORITY Gracefully handle the case where this causes
    // Decimal overflow. (Currently, it will crash, albeit "harmlessly".)
    m_summary_amount_text->SetLabel
    (   finformat_wx
        (   summary_amount(),
            locale(),
            DecimalFormatFlags().clear(string_flags::dash_for_zero)
        )
    );
    Layout();  // This is essential.
    return;
}

size_t
MultiAccountPanel::num_rows() const
{
    size_t const sz = m_account_name_boxes.size();
    JEWEL_ASSERT (sz == m_account_type_boxes.size());
    JEWEL_ASSERT (sz == m_description_boxes.size());
    JEWEL_ASSERT (sz == m_opening_balance_boxes.size());
    return m_account_name_boxes.size();
}

size_t
MultiAccountPanel::num_checked_rows() const
{
    size_t ret = 0;
    for (auto const* check_box: m_check_boxes)
    {
        if (check_box->GetValue()) ++ret;
    }
    return ret;
}

bool
MultiAccountPanel::account_type_is_selected
(   AccountType p_account_type
) const
{
    for (AccountTypeCtrl* const ctrl: m_account_type_boxes)
    {
        if (ctrl->account_type() == p_account_type)
        {
            return true;
        }
    }
    return false;
}

AccountType
MultiAccountPanel::account_type_for(OpeningBalanceCtrl const* p_ctrl) const
{
    JEWEL_ASSERT (p_ctrl->GetParent() == this);
    for (size_t i = 0; i != m_opening_balance_boxes.size(); ++i)
    {
        if (m_opening_balance_boxes[i] == p_ctrl)
        {
            JEWEL_ASSERT (i < m_account_type_boxes.size());
            return m_account_type_boxes[i]->account_type();
        }
    }
    JEWEL_HARD_ASSERT (false);
}

bool
MultiAccountPanel::TransferDataToWindow()
{
    if (!GriddedScrolledPanel::TransferDataToWindow())
    {
        return false;
    }
    update_summary();
    return true;
}

Handle<Account>
MultiAccountPanel::blank_account()
{
    Handle<Account> ret(database_connection());
    wxString const empty_string;
    JEWEL_ASSERT (empty_string.empty());
    ret->set_name(empty_string);
    ret->set_description(empty_string);
    ret->set_visibility(Visibility::visible);
    vector<AccountType> const& atypes =
        account_types(m_account_super_type);
    JEWEL_ASSERT (!atypes.empty());
    ret->set_account_type(atypes.at(0));
    return ret;
}

void
MultiAccountPanel::push_row(Handle<Account> const& p_account)
{
    int const row = current_row();

    // Account name
    TextCtrl* account_name_box = new TextCtrl
    (   this,
        wxID_ANY,
        p_account->name(),
        wxDefaultPosition,
        wxSize(medium_width(), wxDefaultSize.y),
        wxALIGN_LEFT
    );
    top_sizer().Add(account_name_box, wxGBPosition(row, 0));
    m_account_name_boxes.push_back(account_name_box);

    int const height = account_name_box->GetSize().GetY();

    // Account type
    AccountTypeCtrl* account_type_box = new AccountTypeCtrl
    (   this,
        wxID_ANY,
        wxSize(medium_width(), height),
        database_connection(),
        m_account_super_type
    );
    account_type_box->set_account_type(p_account->account_type());
    top_sizer().Add(account_type_box, wxGBPosition(row, 1));
    m_account_type_boxes.push_back(account_type_box);

    // Description
    TextCtrl* description_box = new TextCtrl
    (   this,
        wxID_ANY,
        p_account->description(),
        wxDefaultPosition,
        wxSize(large_width(), height),
        wxALIGN_LEFT
    );
    top_sizer().Add(description_box, wxGBPosition(row, 2), wxGBSpan(1, 2));
    m_description_boxes.push_back(description_box);

    p_account->set_commodity(m_commodity);

    // Opening balance
    OpeningBalanceCtrl* opening_balance_box = new OpeningBalanceCtrl
    (   this,
        wxID_ANY,
        wxSize(medium_width(), height),
        m_commodity->precision(),
        false
    );
    top_sizer().Add(opening_balance_box, wxGBPosition(row, 4));
    m_opening_balance_boxes.push_back(opening_balance_box);

    // Check-box
    CheckBox* check_box = new CheckBox
    (   this,
        wxID_ANY,
        wxEmptyString,
        wxDefaultPosition,
        wxSize(medium_width(), height)
    );
    check_box->SetValue(false);
    top_sizer().Add(check_box, wxGBPosition(row, 5));
    m_check_boxes.push_back(check_box);

    increment_row();

    FitInside();

    return;
}

void
MultiAccountPanel::set_commodity(Handle<Commodity> const& p_commodity)
{
    m_commodity = p_commodity;
    auto const precision = p_commodity->precision();
    for (auto* ctrl: m_opening_balance_boxes)
    {
        // TODO MEDIUM PRIORITY Handle potential Decimal exception here on
        // rounding.
        auto const old_amount = ctrl->amount();
        auto const new_amount = round(old_amount, precision);
        ctrl->set_amount(new_amount);
    }
    return;
}

vector<AugmentedAccount>
MultiAccountPanel::augmented_accounts()
{
    vector<AugmentedAccount> ret;
#   ifndef NDEBUG
        vector<AugmentedAccount>::size_type const original_size =
            ret.size();
#   endif
    vector<AugmentedAccount>::size_type const sz =
        m_account_name_boxes.size();
    JEWEL_ASSERT (m_account_type_boxes.size() == sz);
    JEWEL_ASSERT (m_description_boxes.size() == sz);
    JEWEL_ASSERT (m_opening_balance_boxes.size() == sz);
    vector<AugmentedAccount>::size_type i = 0;
    for ( ; i != sz; ++i)
    {
        AugmentedAccount augmented_account
        (   database_connection(),
            m_commodity
        );
        Handle<Account> const& account = augmented_account.account;
        account->set_name(m_account_name_boxes[i]->GetValue().Trim());
        AccountType const account_type =
            m_account_type_boxes[i]->account_type();
        JEWEL_ASSERT (super_type(account_type) == m_account_super_type);
        account->set_account_type(account_type);
        account->set_description(m_description_boxes[i]->GetValue());
        account->set_visibility(Visibility::visible);
        account->set_commodity(m_commodity);
        augmented_account.technical_opening_balance =
        (   m_account_super_type == AccountSuperType::pl?
            -m_opening_balance_boxes[i]->amount():
            m_opening_balance_boxes[i]->amount()
        );
        JEWEL_ASSERT (!account->has_id());
        ret.push_back(augmented_account);
    }
#   ifndef NDEBUG
        JEWEL_ASSERT (ret.size() == original_size + sz);
#   endif
    return ret;
}

set<wxString>
MultiAccountPanel::selected_account_names() const
{
    set<wxString> ret;
    size_t const sz = m_account_name_boxes.size();
    for (size_t i = 0; i != sz; ++i)
    {
        ret.insert(m_account_name_boxes[i]->GetValue().Trim());    
    }
    return ret;
}

bool
MultiAccountPanel::account_names_valid(wxString& p_error_message) const
{
    JEWEL_LOG_TRACE();
    set<wxString> account_names;
    AccountPhraseFlags const flags =
        AccountPhraseFlags().set(string_flags::capitalize);
    for (TextCtrl* const box: m_account_name_boxes)
    {
        wxString const name = box->GetValue().Trim().Lower();
        if (name.IsEmpty())
        {
            p_error_message =
                account_concept_name(m_account_super_type, flags) +
                wxString(" name is blank");
            return false;
        }
        set<wxString> const* const account_name_sets[] =
            { &account_names, &m_account_names_already_taken };
        for (auto account_name_set: account_name_sets)
        {
            JEWEL_LOG_TRACE();
            if (account_name_set->find(name) != account_name_set->end())
            {
                p_error_message = wxString("Duplicate name: ");
                p_error_message += name;
                return false;
            }
        }
        account_names.insert(name);
    }
    return true;
}

void
MultiAccountPanel::set_account_names_already_taken
(   set<wxString> const& p_account_names_already_taken
)
{
    for (auto name: p_account_names_already_taken)
    {
        m_account_names_already_taken.insert(name.Trim().Lower());
        JEWEL_LOG_VALUE(Log::info, name);
    }
    return;
}

Decimal
MultiAccountPanel::total_amount() const
{
    Decimal ret(0, m_commodity->precision());
    for (auto const& box: m_opening_balance_boxes) ret += box->amount();
    return ret;
}

}  // namespace gui
}  // namespace dcm
