// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "journal.hpp"
#include "entry_handle.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <wx/string.h>
#include <iostream>
#include <ostream>
#include <numeric>
#include <string>
#include <vector>

using jewel::Decimal;
using std::accumulate;
using std::endl;
using std::ostream;
using std::string;
using std::vector;


namespace phatbooks
{


namespace
{
	Decimal entry_accumulation_aux(Decimal const& dec, EntryHandle const& entry)
	{
		return dec + entry->amount();
	}
}  // end anonymous namespace


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
Journal::push_entry(EntryHandle const& entry)
{
	do_push_entry(entry);
	return;
}

void
Journal::remove_entry(EntryHandle const& entry)
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

std::vector<EntryHandle> const&
Journal::entries() const
{
	return do_get_entries();
}

wxString
Journal::comment() const
{
	return do_get_comment();
}

bool
Journal::is_actual() const
{
	return transaction_type_is_actual(transaction_type());
}

TransactionType
Journal::transaction_type() const
{
	return do_get_transaction_type();
}

Decimal
Journal::balance() const
{
	return accumulate
	(	entries().begin(),
		entries().end(),
		Decimal(0, 0),
		entry_accumulation_aux
	);
}

bool
Journal::is_balanced() const
{
	return balance() == Decimal(0, 0);
}

Decimal
Journal::primary_amount() const
{
	Decimal total(0, 0);
	for (EntryHandle const& entry: entries())
	{
		if (entry->transaction_side() == TransactionSide::destination)
		{
			total += entry->amount();
		}
	}
	return is_actual()? total: -total;
}



}  // namespace phatbooks
