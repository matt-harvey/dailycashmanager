// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_column_creation_hpp
#define GUARD_column_creation_hpp

#include "entry.hpp"
#include "account.hpp"
#include "ordinary_journal.hpp"
#include <consolixx/column.hpp>

namespace phatbooks
{

/**
 * Contains functions for creating various consolixx::Column
 * objects, intended for populating consolixx::Table.
 *
 * For the functions here that return a raw pointer (which may
 * be all of them), the returned pointer points to a heap-allocated
 * column object. It is the caller's responsibility to ensure
 * the heap memory is deleted. Typically the client will
 * want to use the raw pointer to initialize a
 * Table<...>::ColumnPtr of the type required for the table they
 * are constructing.
 *
 * See the documentation for consolixx::Table for more information
 * on constructing Tables using Column objects. (Note there
 * is no relationship between such Tables, and the concept
 * of a relational database table; and neither is there any
 * relationship between the concept of a consolixx:Column
 * (ColumnPtr, ColumnBase etc.) and a column in a relational
 * database.)
 */
namespace column_creation
{



/*********************************************************/
// Functions creating columns for use in Tables of Entries.

/**
 * Creates a column that shows the journal id of each Entry's journal.
 * Should not be used unless we KNOW that all the Entry's in the
 * desired range are ordinary (not draft or proto) Entries.
 */
consolixx::PlainColumn<Entry, OrdinaryJournal::Id>*
create_entry_ordinary_journal_id_column();

/**
 * Creates a column that shows the date of each Entry. Should not be
 * used unless we KNOW that all the entries in range are
 * ordinary (not draft or proto) entries.
 */
consolixx::PlainColumn<Entry, boost::gregorian::date>*
create_entry_ordinary_journal_date_column();

/**
 * Creates a column that shows the id of each Entry.
 */
consolixx::PlainColumn<Entry, boost::optional<Entry::Id> >*
create_entry_id_column();

/**
 * Creates a column that shows the name of the account of
 * each Entry.
 */
consolixx::PlainColumn<Entry, wxString>*
create_entry_account_name_column();

/**
 * Creates a column that shows the comment of each Entry.
 */
consolixx::PlainColumn<Entry, wxString>*
create_entry_comment_column();

#ifdef PHATBOOKS_EXPOSE_COMMODITY
	consolixx::PlainColumn<Entry, wxString>*
	create_entry_commodity_abbreviation_column();
#endif

/**
 * Creates a column that shows the amount of each Entry.
 */
consolixx::PlainColumn<Entry, jewel::Decimal>*
create_entry_amount_column();

/**
 * Creates a column that show the amount of each Entry,
 * with the sign switched.
 */
consolixx::PlainColumn<Entry, jewel::Decimal>*
create_entry_reversed_amount_column();

/**
 * Creates a column that displays each Entry's amount and
 * calculates a total to
 * show at the foot of the column.
 */
consolixx::AccumulatingColumn<Entry, jewel::Decimal>*
create_entry_accumulating_amount_column
(	jewel::Decimal const& p_seed = jewel::Decimal(0, 0)
);

/**
 * Creates a column that displays each Entry's reversed amount and
 * calculates a total
 * to show at the foot of the column.
 */
consolixx::AccumulatingColumn<Entry, jewel::Decimal>*
create_entry_accumulating_reversed_amount_column
(	jewel::Decimal const& p_seed = jewel::Decimal(0, 0)
);

/**
 * Creates a column that displays a running total of Entry amounts,
 * but does not show a footer.
 */
consolixx::AccumulatingColumn<Entry, jewel::Decimal>*
create_entry_running_total_amount_column
(	jewel::Decimal const& p_seed = jewel::Decimal(0, 0)
);

/**
 * Creates a column that displays a running total of Entry
 * reconciled amounts, but does not show a footer.
 */
consolixx::AccumulatingColumn<Entry, jewel::Decimal>*
create_entry_running_total_reconciled_amount_column
(	jewel::Decimal const& p_seed = jewel::Decimal(0, 0)
);

/**
 * Creates a column that shows the reconciliation status
 * ("y" or "n") of each Entry.
 */
consolixx::PlainColumn<Entry, bool>*
create_entry_reconciliation_status_column();




/*******************************************/
// Functions for creating columns for use in
// Tables of Accounts.

/**
 * Creates a column showing the name of an Account.
 */
consolixx::PlainColumn<Account, wxString>*
create_account_name_column();

/**
 * Creates a column showing the AccountType of an Account.
 */
consolixx::PlainColumn<Account, account_type::AccountType>*
create_account_type_column();

/**
 * Creates a column showing the description of an Account.
 */
consolixx::PlainColumn<Account, wxString>*
create_account_description_column();

/**
 * Creates a column showing the "friendly balance" of an
 * Account.
 */
consolixx::PlainColumn<Account, jewel::Decimal>*
create_account_friendly_balance_column();

/**
 * Creates a column showing the friendly balance of an
 * Account, with the total of all the friendly balances
 * (of Accounts in the Table) at the foot of the column.
 */
consolixx::AccumulatingColumn<Account, jewel::Decimal>*
create_account_accumulating_friendly_balance_column();

/**
 * Creates a column showing the amalgamated (recurrring) budget
 * for each Account, with the total of all the amalgamated
 * budgets at the foot of the Column. The database
 * connection must be passed to the second paramter so we
 * can get the frequency of the AmalgamatedBudget, which
 * affects the wording of the header.
 */
consolixx::AccumulatingColumn<Account, jewel::Decimal>*
create_account_budget_column
(	PhatbooksDatabaseConnection const& p_database_connection
);

/**
 * Creates a column showing the "friendly" opening balance
 * of an Account.
 */
consolixx::PlainColumn<Account, jewel::Decimal>*
create_account_friendly_opening_balance_column();

}  // namespace column_creation
}  // namespace phatbooks


#endif  // GUARD_column_creation_hpp
