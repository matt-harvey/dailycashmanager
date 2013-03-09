#include "b_string.hpp"
#include "entry.hpp"
#include "entry_impl.hpp"
#include "finformat.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "b_string.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/static_assert.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <consolixx/alignment.hpp>
#include <consolixx/column.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <string>
#include <vector>

using boost::lexical_cast;
using boost::optional;
using boost::shared_ptr;
using consolixx::PlainColumn;
using consolixx::AccumulatingColumn;
using jewel::Decimal;
using jewel::UninitializedOptionalException;
using jewel::value;
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
	// Convenient non-member functions to assist in constructing
	// consolixx::Column objects per below.
	sqloxx::Id col_aux_ordinary_journal_id(Entry const& entry)
	{
		OrdinaryJournal const journal = entry.journal<OrdinaryJournal>();
		if (!journal.has_id())
		{
			throw UninitializedOptionalException
			(	"Journal does not have id."
			);
		}
		assert (journal.has_id());
		BOOST_STATIC_ASSERT
		(	(boost::is_same<OrdinaryJournal::Id, sqloxx::Id>::value)
		);
		return journal.id();
	}
	gregorian::date col_aux_ordinary_journal_date(Entry const& entry)
	{
		return entry.journal<OrdinaryJournal>().date();
	}
	optional<Entry::Id> col_aux_optional_id(Entry const& entry)
	{
		optional<Entry::Id> ret;
		if (entry.has_id()) ret = entry.id();
		return ret;
	}
	string col_aux_optional_id_to_string(optional<Entry::Id> id)
	{
		if (id)
		{
			return lexical_cast<string>(value(id));
		}
		assert (!id);
		return "N/A";
	}
	BString col_aux_account_name(Entry const& entry)
	{
		return entry.account().name();
	}
	BString col_aux_comment(Entry const& entry)
	{
		return entry.comment();
	}
#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		BString col_aux_commodity_abbreviation(Entry const& entry)
		{
			return entry.account().commodity().abbreviation();
		}
#	endif
	Decimal col_aux_amount(Entry const& entry)
	{
		return entry.amount();
	}
	Decimal col_aux_reversed_amount(Entry const& entry)
	{
		Decimal const amount = entry.amount();
		return round(-amount, amount.places());
	}
	bool col_aux_reconciliation_status(Entry const& entry)
	{
		return entry.is_reconciled();
	}
	Decimal col_aux_accumulate_amount
	(	Entry const& entry,
		Decimal& accumulator
	)
	{
		Decimal const ret = entry.amount();
		accumulator += ret;
		return ret;
	}
	Decimal col_aux_accumulate_reversed_amount
	(	Entry const& entry,
		Decimal& accumulator
	)
	{
		Decimal const raw_amount = entry.amount();
		Decimal const ret = round(-raw_amount, raw_amount.places());
		accumulator += ret;
		return ret;
	}
	string col_aux_reconciliation_status_to_string(bool p_is_reconciled)
	{
		return p_is_reconciled? "y": "n";
	}
}  // end anonymous namespace
		

PlainColumn<Entry, Journal::Id>*
Entry::create_ordinary_journal_id_column()
{
	return new PlainColumn<Entry, Journal::Id>
	(	col_aux_ordinary_journal_id,
		"Journal ID",
		alignment::right
	);
}

PlainColumn<Entry, gregorian::date>*
Entry::create_ordinary_journal_date_column()
{
	return new PlainColumn<Entry, gregorian::date>
	(	col_aux_ordinary_journal_date,
		"Date"
	);
}

PlainColumn<Entry, optional<Entry::Id> >*
Entry::create_id_column()
{
	return new PlainColumn<Entry, optional<Id> >
	(	col_aux_optional_id,
		"Entry ID",
		alignment::right,
		col_aux_optional_id_to_string
	);
}

PlainColumn<Entry, BString>*
Entry::create_account_name_column()
{
	return new PlainColumn<Entry, BString>
	(	col_aux_account_name,
		"Account",
		alignment::left,
		bstring_to_std8
	);
}

PlainColumn<Entry, BString>*
Entry::create_comment_column()
{
	return new PlainColumn<Entry, BString>
	(	col_aux_comment,
		"Comment",
		alignment::left,
		bstring_to_std8
	);
}

#ifdef PHATBOOKS_EXPOSE_COMMODITY
	PlainColumn<Entry, BString>*
	Entry::create_commodity_abbreviation_column()
	{
		return new PlainColumn<Entry, BString>
		(	col_aux_commodity_abbreviation,
			"Commodity",
			alignment::left,
			bstring_to_std8
		);
	}
#endif  // PHATBOOKS_EXPOSE_COMMODITY


PlainColumn<Entry, Decimal>*
Entry::create_amount_column()
{
	return new PlainColumn<Entry, Decimal>
	(	col_aux_amount,
		"Amount",
		alignment::right,
		finformat_std8
	);
}

PlainColumn<Entry, Decimal>*
Entry::create_reversed_amount_column()
{
	return new PlainColumn<Entry, Decimal>
	(	col_aux_reversed_amount,
		"Amount",
		alignment::right,
		finformat_std8
	);
}

AccumulatingColumn<Entry, Decimal>*
Entry::create_accumulating_amount_column()
{
	return new AccumulatingColumn<Entry, Decimal>
	(	col_aux_accumulate_amount,
		Decimal(0, 0),
		"Amount",
		alignment::right,
		finformat_std8
	);
}

AccumulatingColumn<Entry, Decimal>*
Entry::create_accumulating_reversed_amount_column()
{
	return new AccumulatingColumn<Entry, Decimal>
	(	col_aux_accumulate_reversed_amount,
		Decimal(0, 0),
		"Amount",
		alignment::right,
		finformat_std8
	);
}

PlainColumn<Entry, bool>*
Entry::create_reconciliation_status_column()
{
	return new PlainColumn<Entry, bool>
	(	col_aux_reconciliation_status,
		"Reconciled?",
		alignment::left,
		col_aux_reconciliation_status_to_string
	);
}

}  // namespace phatbooks

