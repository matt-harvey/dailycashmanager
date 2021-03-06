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

#include "gui/pl_account_entry_list_ctrl.hpp"
#include "account.hpp"
#include "entry.hpp"
#include "gui/locale.hpp"
#include "gui/filtered_entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::Handle;

namespace gregorian = boost::gregorian;


namespace dcm
{
namespace gui
{

namespace
{
    int comment_col_num()
    {
        return 1;
    }
    int amount_col_num()
    {
        return 2;
    }
    int anon_num_columns()
    {
        return 3;
    }

}  // end anonymous namespace


PLAccountEntryListCtrl::PLAccountEntryListCtrl
(   wxWindow* p_parent,
    wxSize const& p_size,
    Handle<Account> const& p_account,
    optional<gregorian::date> const& p_maybe_min_date,
    optional<gregorian::date> const& p_maybe_max_date
):
    FilteredEntryListCtrl
    (   p_parent,
        p_size,
        p_account,
        p_maybe_min_date,
        p_maybe_max_date
    ),
    m_reverse_signs(p_account->account_type() == AccountType::revenue)
{
}

PLAccountEntryListCtrl::~PLAccountEntryListCtrl()
{
}

void
PLAccountEntryListCtrl::do_set_non_date_columns
(   long p_row,
    Handle<Entry> const& p_entry
)
{
    SetItem(p_row, comment_col_num(), p_entry->comment());
    SetItem
    (   p_row,
        amount_col_num(),
        finformat_wx
        (   friendly_amount(p_entry),
            locale(),
            DecimalFormatFlags().clear(string_flags::dash_for_zero)
        )
    );
    JEWEL_ASSERT (num_columns() == 3);
    return;
}

wxString
PLAccountEntryListCtrl::verb() const
{
    switch (account()->account_type())
    {
    case AccountType::revenue:
        return wxString("earned");
    case AccountType::expense:
        return wxString("spent");
    default:
        JEWEL_HARD_ASSERT (false);
    }
}

void
PLAccountEntryListCtrl::do_insert_non_date_columns()
{
    InsertColumn
    (   comment_col_num(),
        wxString("Memo"),
        wxLIST_FORMAT_LEFT
    );
    InsertColumn
    (   amount_col_num(),
        wxString("Amount ") + verb(),
        wxLIST_FORMAT_RIGHT
    );
    JEWEL_ASSERT (num_columns() == 3);
    return;
}

int
PLAccountEntryListCtrl::do_get_comment_col_num() const
{
    return comment_col_num();
}

int
PLAccountEntryListCtrl::do_get_num_columns() const
{
    return anon_num_columns();
}

jewel::Decimal
PLAccountEntryListCtrl::friendly_amount(Handle<Entry> const& p_entry) const
{
    return m_reverse_signs? -p_entry->amount(): p_entry->amount();
}


}  // namespace gui
}  // namespace dcm
