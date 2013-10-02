// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "ordinary_journal_handle.hpp"
#include "account_handle.hpp"
#include "commodity_handle.hpp"
#include "draft_journal_handle.hpp"
#include "entry_handle.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/decimal.hpp>
#include <jewel/log.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <wx/string.h>
#include <utility>

using jewel::Decimal;
using sqloxx::Handle;
using sqloxx::Id;
using std::move;

namespace phatbooks
{

typedef sqloxx::Handle<OrdinaryJournal> OrdinaryJournalHandle;

OrdinaryJournalHandle
create_opening_balance_journal
(	AccountHandle const& p_account,
	Decimal const& p_desired_opening_balance
)
{
	// Normally the implementation of OrdinaryJournal function is
	// delegated to OrdinaryJournal. But here, we are using
	// a static function to return an OrdinaryJournal, which we create using
	// "high level", OrdinaryJournal-level functions; so it seems
	// appropriate to implement it here.
	PhatbooksDatabaseConnection& dbc = p_account->database_connection();
	AccountHandle const balancing_account = dbc.balancing_account();
	Decimal const old_opening_balance =
		p_account->has_id()?
		p_account->technical_opening_balance():
		Decimal(0, p_account->commodity()->precision());
	Decimal const primary_entry_amount =
		p_desired_opening_balance - old_opening_balance;

	OrdinaryJournalHandle const ret(dbc);

	EntryHandle const primary_entry(dbc);
	primary_entry->set_account(p_account);
	primary_entry->set_comment("Opening balance entry");
	primary_entry->set_amount(primary_entry_amount);
	primary_entry->set_whether_reconciled(true);
	primary_entry->set_transaction_side(TransactionSide::source);
	ret->push_entry(primary_entry);

	EntryHandle const balancing_entry(dbc);
	balancing_entry->set_account(balancing_account);
	balancing_entry->set_comment("Opening balance entry");
	balancing_entry->set_amount(-primary_entry_amount);
	balancing_entry->set_whether_reconciled(false);
	balancing_entry->set_transaction_side(TransactionSide::destination);
	ret->push_entry(balancing_entry);

	ret->set_comment("Opening balance adjustment");
	if (p_account->account_super_type() == AccountSuperType::balance_sheet)
	{
		ret->set_transaction_type(TransactionType::generic);
	}
	else
	{
		ret->set_transaction_type(TransactionType::envelope);
	}

	// WARNING The source and destination Accounts are the opposite way
	// round to what would be expected by the user here; but this probably
	// doesn't matter, as the user should never see the opening balance
	// Journals directly.
	ret->set_date_unrestricted
	(	dbc.opening_balance_journal_date()
	);
	return move(ret);
}


}  // namespace phatbooks
