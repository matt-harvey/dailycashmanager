#include "b_string.hpp"
#include "journal.hpp"
#include "entry.hpp"
#include <consolixx/table.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/output_aux.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

using consolixx::Table;
using jewel::Decimal;
using std::endl;
using std::ostream;
using std::string;
using std::vector;

namespace phatbooks
{

Journal::~Journal()
{
}

void
Journal::set_whether_actual(bool p_is_actual)
{
	do_set_whether_actual(p_is_actual);
	return;
}

void
Journal::set_comment(BString const& p_comment)
{
	do_set_comment(p_comment);
	return;
}

void
Journal::push_entry(Entry& entry)
{
	do_push_entry(entry);
	return;
}

void
Journal::remove_entry(Entry& entry)
{
	do_remove_entry(entry);
	return;
}

std::vector<Entry> const&
Journal::entries() const
{
	return do_get_entries();
}

BString
Journal::comment() const
{
	return do_get_comment();
}

bool
Journal::is_actual() const
{
	return do_get_whether_actual();
}

Decimal
Journal::balance() const
{
	typedef vector<Entry>::const_iterator Iter;
	Decimal ret(0, 0);
	for (Iter it = entries().begin(), end = entries().end(); it != end; ++it)
	{
		ret += it->amount();
	}
	return ret;
}
		

bool
Journal::is_balanced() const
{
	return balance() == Decimal(0, 0);
}

void
Journal::push_core_journal_columns(Table<Entry>& table) const
{
	typedef Table<Entry>::ColumnPtr ColumnPtr;
	ColumnPtr const name_column(Entry::create_account_name_column());
	table.push_column(name_column);
	ColumnPtr const comment_column(Entry::create_comment_column());
	table.push_column(comment_column);
#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		ColumnPtr const commodity_column
		(	Entry::create_commodity_abbreviation_column()
		);
		table.push_column(commodity_column);
#	endif
	bool const change_signs = !is_actual();
	if (change_signs)
	{
		ColumnPtr const reversed_amount_column
		(	Entry::create_accumulating_reversed_amount_column()
		);
		table.push_column(reversed_amount_column);
	}
	else
	{
		ColumnPtr const amount_column
		(	Entry::create_accumulating_amount_column()
		);
		table.push_column(amount_column);
	}
	return;
}
	

void
Journal::output_core_journal_header(ostream& os) const
{
	namespace alignment	= consolixx::alignment;
	if (is_actual()) os << "ACTUAL TRANSACTION";
	else os << "BUDGET TRANSACTION";
	os << endl;
	if (!comment().empty()) os << comment() << endl;
	os << endl;

	/*
	Table<Entry> table;
	ColumnPtr const id_column(Entry::create_id_column());
	table.push_column(id_column);
	push_core_journal_columns(table);
	table.populate(entries().begin(), entries().end());
	os << table;
	*/
	return;
}

void
Journal::output_journal_aux(ostream& os, Journal const& journal)
{
	journal.do_output(os);
	return;
}


ostream&
operator<<(ostream& os, Journal const& journal)
{
	jewel::output_aux(os, journal, Journal::output_journal_aux);
	return os;
}



}  // namespace phatbooks
