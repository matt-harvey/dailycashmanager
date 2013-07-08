// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "ordinary_journal.hpp"
#include "account.hpp"
#include "b_string.hpp"
#include "draft_journal.hpp"
#include "draft_journal_impl.hpp"
#include "entry.hpp"
#include "ordinary_journal_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <jewel/output_aux.hpp>
#include <sqloxx/handle.hpp>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using boost::lexical_cast;
using boost::shared_ptr;
using jewel::Decimal;
using jewel::output_aux;
using sqloxx::Handle;
using std::ios_base;
using std::ostream;
using std::ostringstream;
using std::string;
using std::vector;

// For debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;

namespace phatbooks
{

void
OrdinaryJournal::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	OrdinaryJournalImpl::setup_tables(dbc);
	return;
}

OrdinaryJournal::OrdinaryJournal
(	PhatbooksDatabaseConnection& p_database_connection
):
	PhatbooksPersistentObject(p_database_connection)
{
}

OrdinaryJournal::OrdinaryJournal
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
{
}

OrdinaryJournal
OrdinaryJournal::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return OrdinaryJournal
	(	Handle<OrdinaryJournalImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}

OrdinaryJournal
OrdinaryJournal::create_opening_balance_journal
(	Account const& p_account,
	Decimal const& p_desired_opening_balance
)
{
	// Normally the implementation of OrdinaryJournal function is
	// delegated to OrdinaryJournalImpl. But here, we are using
	// a static function to return an OrdinaryJournal, which we create using
	// "high level", OrdinaryJournal-level functions; so it seems
	// appropriate to implement it here.
	PhatbooksDatabaseConnection& dbc = p_account.database_connection();
	Account const balancing_account = dbc.balancing_account();
	Decimal const old_opening_balance =
		p_account.has_id()?
		p_account.technical_opening_balance():
		Decimal(0, p_account.commodity().precision());
	Decimal const primary_entry_amount =
		p_desired_opening_balance - old_opening_balance;

	OrdinaryJournal ret(dbc);

	Entry primary_entry(dbc);
	primary_entry.set_account(p_account);
	primary_entry.set_comment("Opening balance entry");
	primary_entry.set_amount(primary_entry_amount);
	primary_entry.set_whether_reconciled(false);
	ret.push_entry(primary_entry);

	Entry balancing_entry(dbc);
	balancing_entry.set_account(balancing_account);
	balancing_entry.set_comment("Opening balance entry");
	balancing_entry.set_amount(-primary_entry_amount);
	balancing_entry.set_whether_reconciled(false);
	ret.push_entry(balancing_entry);

	ret.set_comment("Opening balance adjustment");
	ret.set_whether_actual
	(	p_account.account_super_type() == account_super_type::balance_sheet
	);
	ret.set_transaction_type(transaction_type::generic_transaction);
	ret.set_date_unrestricted
	(	dbc.opening_balance_journal_date()
	);
	return ret;
}

boost::gregorian::date
OrdinaryJournal::date() const
{
	return impl().date();
}

void
OrdinaryJournal::do_set_whether_actual(bool p_is_actual)
{
	impl().set_whether_actual(p_is_actual);
	return;
}

void
OrdinaryJournal::do_set_transaction_type
(	transaction_type::TransactionType p_transaction_type
)
{
	impl().set_transaction_type(p_transaction_type);
	return;
}

void
OrdinaryJournal::do_set_comment(BString const& p_comment)
{
	impl().set_comment(p_comment);
	return;
}

void
OrdinaryJournal::set_date(boost::gregorian::date const& p_date)
{
	impl().set_date(p_date);
	return;
}

void
OrdinaryJournal::set_date_unrestricted(boost::gregorian::date const& p_date)
{
	OrdinaryJournalSignature const signature;
	impl().set_date_unrestricted(p_date, signature);
	return;
}

void
OrdinaryJournal::do_push_entry(Entry& entry)
{
	impl().push_entry(entry);
	return;
}

void
OrdinaryJournal::do_remove_entry(Entry& entry)
{
	impl().remove_entry(entry);
	return;
}

bool
OrdinaryJournal::do_get_whether_actual() const
{
	return impl().is_actual();
}

transaction_type::TransactionType
OrdinaryJournal::do_get_transaction_type() const
{
	return impl().transaction_type();
}

BString
OrdinaryJournal::do_get_comment() const
{
	return impl().comment();
}

vector<Entry> const&
OrdinaryJournal::do_get_entries() const
{
	return impl().entries();
}

void
OrdinaryJournal::mimic(ProtoJournal const& rhs)
{
	impl().mimic(rhs);
	return;
}

void
OrdinaryJournal::mimic(DraftJournal const& rhs)
{
	impl().mimic(rhs);
	return;
}

void
OrdinaryJournal::do_clear_entries()
{
	impl().clear_entries();
	return;
}

OrdinaryJournal::OrdinaryJournal
(	sqloxx::Handle<OrdinaryJournalImpl> const& p_handle
):
	PhatbooksPersistentObject(p_handle)
{
}


void
OrdinaryJournal::do_output(ostream& os) const
{
	os << date() << " ";
	// lexical_cast here avoids unwanted formatting
	os << "ORDINARY JOURNAL ";
	if (has_id())
	{
		os << "ID " << lexical_cast<string>(id()) << " ";
	}
	PersistentJournal::do_output(os);
	return;
}





}  // namespace phatbooks
