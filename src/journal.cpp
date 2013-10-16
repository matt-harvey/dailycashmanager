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


#include "journal.hpp"
#include "entry.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <wx/string.h>
#include <iostream>
#include <ostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::Handle;
using sqloxx::Id;
using std::accumulate;
using std::endl;
using std::ostream;
using std::string;
using std::vector;


namespace phatbooks
{

struct Journal::JournalData
{
	boost::optional<TransactionType> transaction_type;
	boost::optional<wxString> comment;
	std::vector<Handle<Entry> > entries;
};


namespace
{
	Decimal entry_accumulation_aux
	(	Decimal const& dec,
		Handle<Entry> const& entry
	)
	{
		return dec + entry->amount();
	}
}  // end anonymous namespace

Journal::Journal(): m_data(new JournalData)
{
}

Journal::~Journal() = default;

void
Journal::set_transaction_type
(	TransactionType p_transaction_type
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
	return accumulate
	(	entries().begin(),
		entries().end(),
		Decimal(0, 0),
		entry_accumulation_aux
	);
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

Journal::Journal(Journal const& rhs):
	m_data(new JournalData(*(rhs.m_data)))
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
	// TODO HIGH PRIORITY Make sure this is exception-safe.
	vector<Handle<Entry> > temp = m_data->entries;
	m_data->entries.clear();
	remove_copy
	(	temp.begin(),
		temp.end(),
		back_inserter(m_data->entries),
		entry
	);
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
(	Journal& rhs,
	PhatbooksDatabaseConnection& dbc,
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


}  // namespace phatbooks
