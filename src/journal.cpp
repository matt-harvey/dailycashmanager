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

#include "journal.hpp"
#include "entry.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <wx/string.h>
#include <iostream>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::Handle;
using sqloxx::Id;
using std::endl;
using std::ostream;
using std::string;
using std::unordered_map;
using std::vector;


namespace dcm
{

struct Journal::JournalData
{
    boost::optional<TransactionType> transaction_type;
    boost::optional<wxString> comment;
    std::vector<Handle<Entry> > entries;
};

Journal::Journal(): m_data(new JournalData)
{
}

Journal::~Journal() = default;

void
Journal::set_transaction_type
(   TransactionType p_transaction_type
)
{
    do_set_transaction_type(p_transaction_type);    
    return;
}

void
Journal::set_comment(wxString const& p_comment)
{
    do_set_comment(p_comment);
    return;
}

void
Journal::push_entry(Handle<Entry> const& entry)
{
    do_push_entry(entry);
    return;
}

void
Journal::remove_entry(Handle<Entry> const& entry)
{
    do_remove_entry(entry);
    return;
}

void
Journal::clear_entries()
{
    do_clear_entries();
    return;
}

std::vector<Handle<Entry> > const&
Journal::entries()
{
    return do_get_entries();
}

wxString
Journal::comment()
{
    return do_get_comment();
}

bool
Journal::is_actual()
{
    return transaction_type_is_actual(transaction_type());
}

TransactionType
Journal::transaction_type()
{
    return do_get_transaction_type();
}

Decimal
Journal::balance()
{
    Decimal ret(0, 0);
    for (auto const& entry: entries()) ret += entry->amount();
    return ret;
}

bool
Journal::is_balanced()
{
    return balance() == Decimal(0, 0);
}

Decimal
Journal::primary_amount()
{
    Decimal total(0, 0);
    for (Handle<Entry> const& entry: entries())
    {
        if (entry->transaction_side() == TransactionSide::destination)
        {
            total += entry->amount();
        }
    }
    return is_actual()? total: -total;
}

Journal::Journal(Journal const& rhs): m_data(new JournalData(*(rhs.m_data)))
{
}

Journal::Journal(Journal&&) = default;

void
Journal::swap(Journal& rhs)
{
    using std::swap;
    swap(m_data, rhs.m_data);
    return;
}

std::vector<Handle<Entry> > const&
Journal::do_get_entries()
{
    return m_data->entries;
}

void
Journal::do_set_transaction_type(TransactionType p_transaction_type)
{
    m_data->transaction_type = p_transaction_type;
    return;
}

void
Journal::do_set_comment(wxString const& p_comment)
{
    m_data->comment = p_comment;
    return;
}

void
Journal::do_push_entry(Handle<Entry> const& entry)
{
    m_data->entries.push_back(entry);
    return;
}

void
Journal::do_remove_entry(Handle<Entry> const& entry)
{
    vector<Handle<Entry> > temp;
    remove_copy
    (   m_data->entries.begin(),
        m_data->entries.end(),
        back_inserter(temp),
        entry
    );
    /* TODO LOW PRIORITY On GCC 4.6.1 (at least with MinGW), is_no_throw...
     * are not available.
    static_assert
    (   std::is_nothrow_move_constructible<decltype(temp)>::value &&
        std::is_nothrow_move_assignable<decltype(temp)>::value &&
        std::is_same<decltype(temp), decltype(m_data->entries)>::value,
        "In body of Journal::do_remove_entry, preconditions for nothrow "
        "std::swap are not met."
    );
    */
    using std::swap;
    swap(m_data->entries, temp);
    return;
}

void
Journal::do_clear_entries()
{
    m_data->entries.clear();
    return;
}

wxString
Journal::do_get_comment()
{
    return value(m_data->comment);
}

TransactionType
Journal::do_get_transaction_type()
{
    return value(m_data->transaction_type);
}

void
Journal::mimic_core
(   Journal& rhs,
    DcmDatabaseConnection& dbc,
    optional<Id> id
)
{
    set_transaction_type(rhs.transaction_type());
    set_comment(rhs.comment());
    clear_entries();
    for (Handle<Entry> const& rentry: rhs.entries())
    {
        Handle<Entry> const entry(dbc);
        entry->mimic(*rentry);
        if (id) entry->set_journal_id(value(id));
        push_entry(entry);
    }
    return;
}

void
Journal::clear_core()
{
    jewel::clear(m_data->transaction_type);
    jewel::clear(m_data->comment);
    do_clear_entries();
    return;
}

}  // namespace dcm
