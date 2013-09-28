// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "entry.hpp"
#include "entry_impl.hpp"
#include "finformat.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "string_conv.hpp"
#include "transaction_side.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/static_assert.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/string.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using boost::lexical_cast;
using boost::optional;
using jewel::Decimal;
using jewel::UninitializedOptionalException;
using jewel::value;
using sqloxx::Handle;
using sqloxx::SQLStatement;
using std::move;
using std::unique_ptr;
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
Entry::set_comment(wxString const& p_comment)
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

void
Entry::set_transaction_side
(	transaction_side::TransactionSide p_transaction_side
)
{
	impl().set_transaction_side(p_transaction_side);
	return;
}

wxString
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

transaction_side::TransactionSide
Entry::transaction_side() const
{
	return impl().transaction_side();
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
	
unique_ptr<SQLStatement>
create_date_ordered_actual_ordinary_entry_selector
(	PhatbooksDatabaseConnection& p_database_connection,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date,
	optional<Account> const& p_maybe_account
)
{
	unique_ptr<SQLStatement> ret =
		create_date_ordered_actual_ordinary_entry_selector_aux
		(	p_database_connection,
			p_maybe_min_date,
			p_maybe_max_date,
			p_maybe_account
		);
	return move(ret);
}

}  // namespace phatbooks

