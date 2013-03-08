#include "b_string.hpp"
#include "entry.hpp"
#include "entry_impl.hpp"
#include "finformat.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "b_string.hpp"
#include <consolixx/alignment.hpp>
#include <consolixx/column.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

using boost::lexical_cast;
using boost::shared_ptr;
using consolixx::Column;
using jewel::Decimal;
using sqloxx::Handle;
using std::string;
using std::vector;

namespace alignment = consolixx::alignment;
namespace gregorian = boost::gregorian;

namespace phatbooks
{



class Account;

void
Entry::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	EntryImpl::setup_tables(dbc);
	return;
}


Entry::Entry
(	PhatbooksDatabaseConnection& p_database_connection
):
	PhatbooksPersistentObject(p_database_connection)
{
}

Entry::Entry
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
{
}


Entry
Entry::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return Entry
	(	Handle<EntryImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}


void
Entry::set_journal_id(Journal::Id p_journal_id)
{
	impl().set_journal_id(p_journal_id);
	return;
}

void
Entry::set_account(Account const& p_account)
{
	impl().set_account(p_account);
	return;
}

void
Entry::set_comment(BString const& p_comment)
{
	impl().set_comment(p_comment);
	return;
}

void
Entry::set_amount(jewel::Decimal const& p_amount)
{
	impl().set_amount(p_amount);
	return;
}

void
Entry::set_whether_reconciled(bool p_is_reconciled)
{
	impl().set_whether_reconciled(p_is_reconciled);
	return;
}

BString
Entry::comment() const
{
	return impl().comment();
}

jewel::Decimal
Entry::amount() const
{
	return impl().amount();
}

Account
Entry::account() const
{
	return impl().account();
}

bool
Entry::is_reconciled() const
{
	return impl().is_reconciled();
}

gregorian::date
Entry::date() const
{
	return journal<OrdinaryJournal>().date();
}


void
Entry::mimic(Entry const& rhs)
{
	impl().mimic(rhs.impl());
	return;
}


Entry::Entry(sqloxx::Handle<EntryImpl> const& p_handle):
	PhatbooksPersistentObject(p_handle)
{
}


namespace
{
	// Convenient non-member functions for initializing consolixx::Columns.
	// "mcs" stands for "MakeCellString" and helps these functions to
	// stand out as "unusual".
	string mcs_entry_ordinary_journal_id(Entry const& entry)
	{
		OrdinaryJournal const journal(entry.journal<OrdinaryJournal>());
		return lexical_cast<string>(journal.id());
	}
	string mcs_entry_ordinary_journal_date(Entry const& entry)
	{
		OrdinaryJournal const journal(entry.journal<OrdinaryJournal>());
		return lexical_cast<string>(journal.date());
	}
	string mcs_entry_id(Entry const& entry)
	{
		if (entry.has_id())
		{
			return lexical_cast<string>(entry.id());
		}
		assert (!entry.has_id());
		return "N/A";
	}
	string mcs_entry_account_name(Entry const& entry)
	{
		return bstring_to_std8(entry.account().name());
	}
	string mcs_entry_comment(Entry const& entry)
	{
		return bstring_to_std8(entry.comment());
	}
#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		string mcs_entry_commodity_abbreviation(Entry const& entry)
		{
			return bstring_to_std8
			(	entry.account().commodity().abbreviation()
			);
		}
#	endif
	string mcs_entry_amount(Entry const& entry)
	{
		return finformat_std8(entry.amount());
	}
	string mcs_entry_reversed_amount(Entry const& entry)
	{
		Decimal const amount = entry.amount();
		Decimal::places_type const places = amount.places();
		return finformat_std8(round(-amount, places));
	}
	string mcs_entry_reconciliation_status(Entry const& entry)
	{
		return entry.is_reconciled()? "y": "n";
	}
}  // end anonymous namespace
		

Column<Entry>
Entry::create_ordinary_journal_id_column()
{
	return Column<Entry>
	(	mcs_entry_ordinary_journal_id,
		"Journal ID",
		alignment::right
	);
}

Column<Entry>
Entry::create_ordinary_journal_date_column()
{
	return Column<Entry>(mcs_entry_ordinary_journal_date, "Date");
}

Column<Entry>
Entry::create_id_column()
{
	return Column<Entry>(mcs_entry_id, "Entry ID", alignment::right);
}

Column<Entry>
Entry::create_account_name_column()
{
	return Column<Entry>(mcs_entry_account_name, "Account");
}

Column<Entry>
Entry::create_comment_column()
{
	return Column<Entry>(mcs_entry_comment, "Comment");
}


#ifdef PHATBOOKS_EXPOSE_COMMODITY
	Column<Entry>
	Entry::create_commodity_abbreviation_column()
	{
		return Column<Entry>(mcs_entry_commodity_abbreviation, "Commodity");
	}
#endif  // PHATBOOKS_EXPOSE_COMMODITY


Column<Entry>
Entry::create_amount_column()
{
	return Column<Entry>(mcs_entry_amount, "Amount", alignment::right);
}

Column<Entry>
Entry::create_reversed_amount_column()
{
	return Column<Entry>
	(	mcs_entry_reversed_amount,
		"Amount",
		alignment::right
	);
}

Column<Entry>
Entry::create_reconciliation_status_column()
{
	return Column<Entry>(mcs_entry_reconciliation_status, "Reconciled?");
}

}  // namespace phatbooks

