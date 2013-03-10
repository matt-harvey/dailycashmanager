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
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <string>
#include <vector>

using boost::lexical_cast;
using boost::optional;
using boost::shared_ptr;
using jewel::Decimal;
using jewel::UninitializedOptionalException;
using jewel::value;
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


}  // namespace phatbooks

