// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_ordinary_journal_handle_hpp_8344533833835522
#define GUARD_ordinary_journal_handle_hpp_8344533833835522

#include "account_handle_fwd.hpp"
#include "ordinary_journal_impl.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>

namespace phatbooks
{

typedef
	sqloxx::Handle<OrdinaryJournalImpl>
	OrdinaryJournalHandle;

/**
 * @returns a Handle to an OrdinaryJournal which, <em> if saved </em>,
 * will adjust the technical opening balance of p_account to become
 * p_desired_opening_balance. The journal will be an actual
 * (non-budget) journal if p_account is a balance sheet Account;
 * otherwise, if p_account is a P&L account, it will be
 * a budget journal.
 *
 * The OrdinaryJournal should not be saved unless and until
 * p_account has an id (i.e. is persisted to the database).
 *
 * Between creating the OrdinaryJournal from this
 * function, and saving it, there should be no
 * other adjustments made to the opening balances of
 * Accounts.
 *
 * Note the returned OrdinaryJournal will be ready to save immediately;
 * all of its attributes will be in a valid state. In particular, it
 * will be marked as \e reconciled.
 */
OrdinaryJournalHandle create_opening_balance_journal
(	AccountHandle const& p_account,
	jewel::Decimal const& p_desired_opening_balance
);




}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_handle_hpp_8344533833835522
