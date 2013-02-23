#include "b_string.hpp"
#include "entry.hpp"
#include "entry_impl.hpp"
#include "finformat.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "b_string.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

using boost::lexical_cast;
using boost::shared_ptr;
using jewel::Decimal;
using sqloxx::Handle;
using std::string;
using std::vector;

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
	shared_ptr<vector<string> > make_entry_row_aux
	(	Entry const& entry,
		bool reverse,
		bool make_augmented_ordinary
	)
	{
		shared_ptr<vector<string> > ret(new vector<string>);
		// TODO Here and in the journal printing method, I
		// should add journal_id as either the first or second
		// column.
		if (make_augmented_ordinary)
		{
			OrdinaryJournal journal(entry.journal<OrdinaryJournal>());
			ret->push_back(lexical_cast<string>(journal.date()));
			ret->push_back(lexical_cast<string>(journal.id()));
		}
		if (entry.has_id())
		{
			ret->push_back(lexical_cast<string>(entry.id()));
		}
		else
		{
			ret->push_back("N/A");
		}
		ret->push_back(bstring_to_std8(entry.account().name()));
		ret->push_back(bstring_to_std8(entry.comment()));
		ret->push_back
		(	bstring_to_std8(entry.account().commodity().abbreviation())
		);
		Decimal amount = entry.amount();
		if (reverse)
		{
			Decimal::places_type const places = amount.places();
			amount = round(-amount, places);
		}
		ret->push_back(finformat_std8(amount));
		ret->push_back(entry.is_reconciled()? "y": "n");
		return ret;
	}


}  // End anonymous namespace


shared_ptr<vector<string> >
make_entry_row(Entry const& entry)
{
	return make_entry_row_aux(entry, false, false);
}


shared_ptr<vector<string> >
make_reversed_entry_row(Entry const& entry)
{
	return make_entry_row_aux(entry, true, false);
}


shared_ptr<vector<string> >
make_augmented_ordinary_entry_row(Entry const& entry)
{
	return make_entry_row_aux(entry, false, true);
}


}  // namespace phatbooks

