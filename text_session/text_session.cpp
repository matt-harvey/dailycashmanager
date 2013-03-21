#include "text_session.hpp"


/** \file text_session.cpp
 *
 * \brief Source file for text/console user interface code for Phatbooks.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account.hpp"
#include "account_reader.hpp"
#include "account_type.hpp"
#include "application.hpp"
#include "column_creation.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "draft_journal_reader.hpp"
#include "entry.hpp"
#include "entry_reader.hpp"
#include "filename_validation.hpp"
#include "finformat.hpp"
#include "frequency.hpp"
#include "journal.hpp"
#include "ordinary_journal.hpp"
#include "ordinary_journal_reader.hpp"
#include "persistent_journal.hpp"
#include "proto_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include "b_string.hpp"
#include <consolixx/column.hpp>
#include <consolixx/get_input.hpp>
#include <consolixx/menu.hpp>
#include <consolixx/menu_item.hpp>
#include <consolixx/table.hpp>
#include <sqloxx/database_connection.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/optional.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <boost/ref.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <algorithm>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <sstream>  // for ostringstream
#include <string>
#include <vector>

using consolixx::Column;
using consolixx::get_date_from_user;
using consolixx::get_user_input;
using consolixx::get_constrained_user_input;
using consolixx::get_decimal_from_user;
using consolixx::Menu;
using consolixx::MenuItem;
using consolixx::Table;
using jewel::Decimal;
using jewel::DecimalRangeException;
using jewel::value;
using sqloxx::DatabaseConnection;
using sqloxx::SQLiteException;
using boost::algorithm::split;
using boost::bad_lexical_cast;
using boost::bind;
using boost::cref;
using boost::lexical_cast;
using boost::optional;
using boost::shared_ptr;
using boost::ref;
using boost::regex;
using boost::regex_match;
using sqloxx::BadIdentifier;
using sqloxx::DatabaseTransaction;
using std::cout;
using std::clog;
using std::endl;
using std::find;
using std::list;
using std::locale;
using std::map;
using std::ostringstream;
using std::string;
using std::vector;

namespace alignment = consolixx::alignment;
namespace gregorian = boost::gregorian;


// TODO Enable display of account and envelope balances at a given
// date

// TODO Enable display of account movements for all accounts (esp. P&L)
// in a given date range.



namespace phatbooks
{
namespace tui
{




/********* ANONYMOUS INNER NAMESPACE *****************/

namespace
{
	bool has_entry_with_id_string
	(	PersistentJournal const& journal,
		string const& str
	)
	{
		try
		{
			Entry::Id const id = lexical_cast<Entry::Id>(str);
			return has_entry_with_id(journal, id);
		}
		catch (bad_lexical_cast&)
		{
			return false;
		}
	}

	typedef bool (*AccountValidator)(Account const& account);

	string validator_description(AccountValidator validator)
	{
		string const ret =
		(	validator == is_asset_or_liability?
			"asset or liability account":
			validator == is_balance_sheet_account?
			"asset, liability or equity account":
			validator == is_expense?
			"expense category":
			validator == is_revenue?
			"revenue category":
			validator == is_pl_account?
			"envelope (revenue, expense or pure envelope)":  // TODO This description sucks
			validator == is_not_pure_envelope?
			"account or category for this transaction":  // TODO This description sucks
			"ERROR"
		);
		assert (ret != "ERROR");
		return ret;
	}

	optional<Entry> elicit_existing_entry(PersistentJournal const& journal)
	{
		string id_string;
		for
		(	vector<Entry>::const_iterator it = journal.entries().begin(),	
				end = journal.entries().end();
			it != end;
			++it
		)
		{
			if (it->has_id())
			{
				Entry::Id const current_id = it->id();
				id_string += lexical_cast<string>(current_id);
				id_string += " ";
			}
		}
		string input = get_constrained_user_input
		(	bind(has_entry_with_id_string, cref(journal), _1),
			"Transaction does not contain a line with this entry id. "
				"Try again, entering one of the following ids:\n" +
				id_string +
				"\nor hit Enter to abort: "
		);
		if (input.empty())
		{
			return optional<Entry>();
		}
		assert (!input.empty());
		return Entry
		(	journal.database_connection(),
			lexical_cast<Entry::Id>(input)
		);
	}

	Decimal elicit_constrained_amount
	(	Commodity const& commodity,
		string const& transaction_description
	)
	{
		Decimal amount;
		for (bool input_is_valid = false; !input_is_valid; )
		{
			cout << "Enter amount " << transaction_description
#			ifdef PHATBOOKS_EXPOSE_COMMODITY
			     << " (in units of " << commodity.abbreviation()
				 << ")"
#			endif
				 << ": ";
			amount = value(get_decimal_from_user());
			Decimal::places_type const initial_precision = amount.places();
			try
			{
				amount = jewel::round(amount, commodity.precision());
				input_is_valid = true;
				if (amount.places() < initial_precision)
				{
					cout << "Amount rounded to " << amount << "." << endl;
				}
			}
			catch (DecimalRangeException&)
			{
				cout << "The number you entered cannot be safely rounded to"
					 << " the required precision of "
					 << commodity.precision()
					 << " decimal places. Please try again."
					 << endl;
				assert (!input_is_valid);
			}
		}
		return amount;
	}

	// TODO This function is weird and misleading in what it does, and
	// it doesn't really belong here.
	bool identifies_existent_journal
	(	PhatbooksDatabaseConnection* dbc,
		string const& s
	)
	{
		try
		{
			Journal::Id const id = lexical_cast<Journal::Id>(s);
			return journal_id_exists(*dbc, id);
		}
		catch (bad_lexical_cast&)
		{
			return false;
		}
	}

	void print_bs_account_reader
	(	BalanceSheetAccountReader const& p_reader,
		bool show_total = true
	)
	{
		JEWEL_DEBUG_LOG << "Printing BalanceSheetAccountReader." << endl;
		Table<Account> table;
		typedef Table<Account>::ColumnPtr ColumnPtr;
		namespace col = column_creation;
		ColumnPtr const name_column(col::create_account_name_column());
		table.push_column(name_column);
		if (show_total)
		{
			ColumnPtr const accumulating_friendly_balance_column
			(	col::create_account_accumulating_friendly_balance_column()
			);
			table.push_column(accumulating_friendly_balance_column);
		}
		else
		{
			ColumnPtr const friendly_balance_column
			(	col::create_account_friendly_balance_column()
			);
			table.push_column(friendly_balance_column);
		}
		table.populate(p_reader.begin(), p_reader.end());
		cout << table;
		return;
	}

	void print_pl_account_reader
	(	PLAccountReader const& p_reader,
		PhatbooksDatabaseConnection const& p_database_connection,
		bool show_total = true
	)
	{
		// TODO Factor out code common to here and print_bs_account_reader
		JEWEL_DEBUG_LOG << "Printing PLAccountReader." << endl;
		Table<Account> table;
		typedef Table<Account>::ColumnPtr ColumnPtr;
		namespace col = column_creation;
		ColumnPtr const name_column(col::create_account_name_column());
		name_column->set_header("Category");
		table.push_column(name_column);
		if (show_total)
		{
			ColumnPtr const accumulating_friendly_balance_column
			(	col::create_account_accumulating_friendly_balance_column()
			);
			table.push_column(accumulating_friendly_balance_column);
		}
		else
		{
			ColumnPtr const friendly_balance_column
			(	col::create_account_friendly_balance_column()
			);
			table.push_column(friendly_balance_column);
		}
		ColumnPtr const budget_column
		(	col::create_account_budget_column(p_database_connection)
		);
		table.push_column(budget_column);
		table.populate(p_reader.begin(), p_reader.end());
		cout << table;
		return;
	}

	void summarise_balance_movement
	(	Account const& account,
		Decimal const& opening_balance,
		Decimal const& closing_balance
	)
	{
		// TODO Are these expressed in terms of the "friendly balances"?
		// Should they be?
		switch (account.account_type())
		{
		case account_type::asset:
		case account_type::liability:
		case account_type::equity:
			cout << "Opening balance for "
			     << account.name() << ": "
				 << finformat_std8(opening_balance) << endl;
			cout << "Movement in balance during date range: "
				 << finformat_std8(closing_balance - opening_balance) << endl;
			cout << "Closing balance: " << finformat_std8(closing_balance)
			     << endl;
			break;
		case account_type::pure_envelope:
			// TODO Do we want some special message here?
			// Fall through
		case account_type::expense:
			cout << "Amount spent in period on " << account.name()
			     << ": " << finformat_std8(closing_balance - opening_balance)
				 << endl;
			break;
		case account_type::revenue:
			cout << "Amount earned in period in " << account.name()
			     << ": " << finformat_std8(closing_balance - opening_balance)
				 << endl;
			break;
		default:
			assert (false);
		}
		return;
	}	

	bool no_persistent_journals_saved(PhatbooksDatabaseConnection& dbc)
	{
		return
			DraftJournal::none_saved(dbc) &&
			OrdinaryJournal::none_saved(dbc);
	}

	bool no_ordinary_entries_saved(PhatbooksDatabaseConnection& dbc)
	{
		if (OrdinaryJournal::none_saved(dbc))
		{
			return true;
		}
		assert (!OrdinaryJournal::none_saved(dbc));
		OrdinaryJournalReader reader(dbc);
		for
		(	OrdinaryJournalReader::const_iterator it = reader.begin();
			it != reader.end();
			++it
		)
		{
			if (!it->entries().empty())
			{
				return false;
			}
		}
		return true;
	}

	bool no_balance_sheet_accounts_saved(PhatbooksDatabaseConnection& dbc)
	{
		return Account::none_saved_with_account_super_type
		(	dbc,
			account_super_type::balance_sheet
		);
	}

	bool no_pl_accounts_saved(PhatbooksDatabaseConnection& dbc)
	{
		return Account::none_saved_with_account_super_type
		(	dbc,
			account_super_type::pl
		);
	}

	bool fewer_than_two_accounts_saved(PhatbooksDatabaseConnection& dbc)
	{
		AccountReader reader(dbc);
		return reader.size() < 2;
	}

	bool account_types_missing_for_expenditure_transaction
	(	PhatbooksDatabaseConnection& dbc
	)
	{
		return
			no_balance_sheet_accounts_saved(dbc) ||
			Account::none_saved_with_account_type
			(	dbc,
				account_type::expense
			);
	}
	
	bool account_types_missing_for_revenue_transaction
	(	PhatbooksDatabaseConnection& dbc
	)
	{
		return
			no_balance_sheet_accounts_saved(dbc) ||
			Account::none_saved_with_account_type
			(	dbc,
				account_type::revenue
			);
	}
		
		
				
}  

/********* END ANONYMOUS INNER NAMESPACE *************/





TextSession::TextSession():
	m_main_menu(new Menu)
{
}


TextSession::~TextSession()
{
}

void
TextSession::create_main_menu()
{
#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		shared_ptr<MenuItem> elicit_commodity_item
		(	new MenuItem
			(	"New commodity",
				bind(&TextSession::elicit_commodity, this),
				true,
				"c"
			)
		);
		m_main_menu->push_item(elicit_commodity_item);
#	endif

	shared_ptr<MenuItem> elicit_account_item
	(	new MenuItem
		(	"New account",
			bind(&TextSession::elicit_account, this),
			true,
			"a"
		)
	);
	elicit_account_item->set_hiding_condition
	(	bind(Commodity::none_saved, ref(database_connection()))
	);
	m_main_menu->push_item(elicit_account_item);

	shared_ptr<MenuItem> elicit_journal_item
	(	new MenuItem
		(	"New transaction",
			bind(&TextSession::elicit_journal, this),
			true,
			"t"
		)
	);
	elicit_journal_item->set_hiding_condition
	(	bind
		(	fewer_than_two_accounts_saved,
			ref(database_connection())
		)
	);
	m_main_menu->push_item(elicit_journal_item);

	shared_ptr<MenuItem> display_draft_journals_item
	(	new MenuItem
		(	"View draft and recurring transactions",
			bind(&TextSession::display_draft_journals, this),
			true,
			"v"
		)
	);
	display_draft_journals_item->set_hiding_condition
	(	bind(DraftJournal::none_saved, ref(database_connection()))
	);
	m_main_menu->push_item(display_draft_journals_item);

	// TODO The wording "Select a transaction by ID" would not be clear for
	// the user as to whether we are referring to a PersistentJournal id,
	// or to an Entry id. We a need a user-facing vocabulary that is going
	// to distinguish between these two, while also not sounding too
	// technical or confusing. For now we have settled on "Select
	// a transaction by journal ID".
	//
	// WARNING This crashes if the menu item is even \e selected, before
	// any journals have been posted.
	shared_ptr<MenuItem> display_journal_from_id_item
	(	new MenuItem
		(	"Select a transaction by journal ID",
			bind(&TextSession::display_journal_from_id, this),
			true,
			"i"
		)
	);
	display_journal_from_id_item->set_hiding_condition
	(	bind(no_persistent_journals_saved, ref(database_connection()))
	);
	m_main_menu->push_item(display_journal_from_id_item);

	shared_ptr<MenuItem> display_ordinary_actual_entries_item
	(	new MenuItem
		(	"List actual transactions",
			bind
			(	&TextSession::display_ordinary_actual_entries,
				this
			),
			true,
			"l"
		)
	);
	display_ordinary_actual_entries_item->set_hiding_condition
	(	bind(no_ordinary_entries_saved, ref(database_connection()))
	);
	m_main_menu->push_item(display_ordinary_actual_entries_item);

	shared_ptr<MenuItem> display_balance_sheet_item
	(	new MenuItem
		(	"Display the balances of asset and liability accounts",
			bind(&TextSession::display_balance_sheet, this),
			true,
			"b"
		)
	);
	display_balance_sheet_item->set_hiding_condition
	(	bind(no_balance_sheet_accounts_saved, ref(database_connection()))
	);
	m_main_menu->push_item(display_balance_sheet_item);

	shared_ptr<MenuItem> display_envelopes_item
	(	new MenuItem
		(	"Display envelope balances",
			bind(&TextSession::display_envelopes, this),
			true,
			"e"
		)
	);
	display_envelopes_item->set_hiding_condition
	(	bind(no_pl_accounts_saved, ref(database_connection()))
	);
	m_main_menu->push_item(display_envelopes_item);

	shared_ptr<MenuItem> perform_reconciliation_item
	(	new MenuItem
		(	"Perform account reconciliation",
			bind(&TextSession::conduct_reconciliation, this),
			true,
			"r"
		)
	);
	perform_reconciliation_item->set_hiding_condition
	(	bind(no_balance_sheet_accounts_saved, ref(database_connection()))
	);
	m_main_menu->push_item(perform_reconciliation_item);

	shared_ptr<MenuItem> display_account_detail_item
	(	new MenuItem
		(	"Account and category detail",
			bind(&TextSession::display_account_detail, this),
			true,
			"ad"
		)
	);
	display_account_detail_item->set_hiding_condition
	(	bind(Account::none_saved, ref(database_connection()))
	);
	m_main_menu->push_item(display_account_detail_item);

	shared_ptr<MenuItem> edit_account_detail_item
	(	new MenuItem
		(	"Edit account detail",
			bind(&TextSession::conduct_account_editing, this),
			true,
			"ea"
		)
	);
	edit_account_detail_item->set_hiding_condition
	(	bind(Account::none_saved, ref(database_connection()))
	);
	m_main_menu->push_item(edit_account_detail_item);

	shared_ptr<MenuItem> quit_item
	(	new MenuItem
		(	"Quit",
			bind(&TextSession::wrap_up, this),
			false,
			"x"
		)
	);
	m_main_menu->push_item(quit_item);

	return;
}

int
TextSession::do_run()
{
	// TODO Tidy this ugly control flow.
	// TODO Make sure this control flow actually works the way it should.
	start:
		// TODO Allow for the omit the extension when on Windows?
		string filepath_str;
		for (bool have_valid_filename = false; !have_valid_filename; )
		{
			cout << "Enter name of file to open: ";
			filepath_str = get_user_input();
			boost::filesystem::path const filepath(filepath_str);
			string const filename = filepath.filename().string();
			string error_message;
			if (is_valid_filename(filename, error_message))
			{
				assert (error_message.empty());
				have_valid_filename = true;
			}
			else
			{
				assert (!error_message.empty());
				cout << "Cannot open file with this name. "
					 << error_message << endl;
				assert (!have_valid_filename);
			}
		}
		boost::filesystem::path const final_filepath(filepath_str);
		int const result = run_with_filepath(final_filepath);
		if (result == 1)
		{
			// Couldn't open filepath
			cout << "Could not open this file." << endl;
			cout << "Try again with another file? (y/n): ";
			string const response = get_constrained_user_input
			(	boost::lambda::_1 == "y" || boost::lambda::_1 == "n",
				"Enter 'y' to try opening another file, or 'n' to abort: ",
				false
			);
			if (response != "y")
			{
				assert (response == "n");
				cout << "Exiting program." << endl;
				return 0;
			}
			assert (response == "y");
			goto start;
		}
		else
		{
			assert (result == 0);
			return result;
		}
}

int
TextSession::do_run(string const& filepath_str)
{
	boost::filesystem::path filepath(filepath_str);
	string const filename = filepath.filename().string();
	string error_message;
	if (is_valid_filename(filename, error_message))
	{
		boost::filesystem::path const final_filepath(filepath_str);
		return run_with_filepath(final_filepath);
	}
	assert (!error_message.empty());
	cout << "Cannot open file with this name. "
	     << error_message << endl;
	return 1;
}


int
TextSession::run_with_filepath
(	boost::filesystem::path const& filepath
)
{
	if (!boost::filesystem::exists(boost::filesystem::status(filepath)))
	{
		cout << "File does not exist. "
		     << "Create file \"" << filepath.string() << "\"? (y/n): ";
		string const response = get_constrained_user_input
		(	boost::lambda::_1 == "y" || boost::lambda::_1 == "n",
			"Try again, entering 'y' to create file, or 'n' to abort: ",
			false
		);
		if (response != "y")
		{
			assert (response == "n");
			cout << "Exiting program." << endl;
			return 0;
		}
		assert (response == "y");
	}
	try
	{
		database_connection().open(filepath);
	}
	catch (SQLiteException&)
	{
		cout << "Could not open file \"" << filepath.string() << "\"."
		     << endl;
		return 1;
	}

	cout << "Welcome to "
	     << bstring_to_std8(Application::application_name())
		 << "!" << endl;

	database_connection().setup();
	gregorian::date const today = gregorian::day_clock::local_day();
	shared_ptr<list<OrdinaryJournal> > auto_posted_journals =
		update_repeaters_till(today);
	notify_autoposts(auto_posted_journals);
	create_main_menu();
	m_main_menu->present_to_user();	
	return 0;
}


string
TextSession::elicit_existing_account_name(bool accept_empty)
{
	 while (true)
	 {
		string input = get_user_input();
	 	if
		(	Account::exists(database_connection(), std8_to_bstring(input)) ||
			(accept_empty && input.empty())
		)
		{
			return input;
		}
		cout << "There is no account named " << input
		     << ". Please try again: ";
	}
}



void TextSession::display_draft_journals()
{
	typedef TextSession PTS;  // For brevity in the below

	// We need this loop here so the menu will update itself
	// if a journal is deleted.
	for (bool exiting = false; !exiting; )
	{
		cout << endl;	
		Menu menu
		(	"Select a transaction to view from the above menu, "
			"or 'x' to exit: "
		);
		DraftJournalReader const dj_reader(database_connection());
		for
		(	DraftJournalReader::const_iterator it = dj_reader.begin();
			it != dj_reader.end();
			++it
		)
		{
			// TODO Prevent the AMALGAMATED BUDGET JOURNAL from
			// being shown to the user.
			shared_ptr<MenuItem const> const menu_item
			(	new MenuItem
				(	bstring_to_std8(it->name()),
					bind
					(	bind(&PTS::conduct_draft_journal_editing, this, _1),
						*it
					)
				)
			);
			menu.push_item(menu_item);
		}
		shared_ptr<MenuItem> exit_item(MenuItem::provide_menu_exit());
		menu.push_item(exit_item);
		menu.present_to_user();
		if (menu.last_choice() == exit_item)
		{
			exiting = true;
		}
	}
	return;
}


void
TextSession::elicit_entry_insertion(PersistentJournal& journal)
{
	Entry entry(database_connection());
	cout << "Enter name of account or category for new transaction line: ";
	TransactionType const transaction_type =
		journal.is_actual()?
		generic_transaction:
		envelope_transaction;
	entry.set_account
	(	value(elicit_valid_account(transaction_type, primary_phase))
	);
	Commodity const commodity = entry.account().commodity();
	Decimal const amount = elicit_constrained_amount
	(	commodity,
		dialogue_phrase(generic_transaction, amount_prompt)
	);
	bool const sign_needs_changing = !journal.is_actual();
	entry.set_amount(sign_needs_changing? -amount: amount);
	cout << "Comment for this line (or Enter for no comment): ";
	entry.set_comment(std8_to_bstring(get_user_input()));
	entry.set_whether_reconciled(false);
	journal.push_entry(entry);
	return;
}



void
TextSession::elicit_entry_deletion(PersistentJournal& journal)	
{
	cout << "Enter the entry id of the transaction line you wish to delete, "
	     << "or just hit Enter to abort: ";
	optional<Entry> maybe_entry = elicit_existing_entry(journal);
	if (!maybe_entry)
	{
		cout << "Entry deletion aborted" << endl;
		return;
	}
	assert (maybe_entry);
	Entry entry(value(maybe_entry));
	journal.remove_entry(entry);
	cout << "Entry deleted.\n" << endl;
	return;
}



void
TextSession::elicit_entry_amendment(PersistentJournal& journal)
{
	cout << "Enter the entry id of the transaction line you wish to amend, "
	     << "or just hit Enter to abort: ";
	optional<Entry> const maybe_entry = elicit_existing_entry(journal);
	if (!maybe_entry)
	{
		cout << "Entry amendment aborted.\n" << endl;
		return;
	}
	Entry entry = value(maybe_entry);

	// Edit account
	cout << "Enter name of new account (or Enter to leave unchanged): ";
	TransactionType const transaction_type =
		journal.is_actual()?
		generic_transaction:
		envelope_transaction;
	optional<Account> maybe_new_account = elicit_valid_account
	(	transaction_type,
		primary_phase,
		true
	);
	if (maybe_new_account)
	{
		entry.set_account(value(maybe_new_account));
	}

	// Edit comment
 	cout << "Enter new comment for this line (or Enter to leave unchanged): ";
	string const new_comment = get_user_input();
	if (!new_comment.empty()) entry.set_comment(std8_to_bstring(new_comment));

	// Edit amount
	for (bool input_is_valid = false; !input_is_valid; )
	{
		cout << "Enter new amount for this line "
		     << "(or Enter to leave unchanged): ";
		optional<Decimal> const maybe_new_amount =
			get_decimal_from_user(true);
		if (!maybe_new_amount)
		{
			input_is_valid = true;
		}
		else
		{
			// TODO The below is very similar to code the
			// function elicit_constrained_amount. Can I factor
			// this out?
			Decimal new_amount = value(maybe_new_amount);
			Decimal::places_type const initial_precision =
				new_amount.places();
			Commodity const commodity = entry.account().commodity();
			try
			{
				new_amount = jewel::round(new_amount, commodity.precision());
				bool const sign_needs_changing = !journal.is_actual();
				entry.set_amount
				(	sign_needs_changing? -new_amount: new_amount
				);
				input_is_valid = true;
				if (new_amount.places() < initial_precision)
				{
					cout << "Amount rounded to " << new_amount << "." << endl;
				}
			}
			catch (DecimalRangeException&)
			{
				cout << "The number you entered cannot be safely "
					 << "rounded to the required precison of "
					 << commodity.precision()
					 << " decimal places. Please try again."
					 << endl;
				assert (!input_is_valid);
			}
		}
	}
	return;
}


void
TextSession::elicit_journal_deletion(PersistentJournal& journal)
{
	cout << "Are you sure you want to delete this entire transaction? (y/n) ";
	string const confirmation = get_constrained_user_input
	(	boost::lambda::_1 == "y" || boost::lambda::_1 == "n",
		"Try again, entering \"y\" to delete or \"n\" to keep: ",
		false
	);
	if (confirmation == "y")
	{
		journal.remove();
		cout << "\nTransaction deleted." << endl << endl;
	}
	else
	{
		assert (confirmation == "n");
		cout << "\nTransaction has not been deleted." << endl << endl;
	}
	return;
}



	
void
TextSession::elicit_comment_amendment
(	PersistentJournal& journal
)
{
	cout << "Enter new comment for this transaction: ";
	journal.set_comment(std8_to_bstring(get_user_input()));
	return;
}


void
TextSession::elicit_repeater_insertion(DraftJournal& journal)
{
	Repeater repeater = elicit_repeater();
	journal.push_repeater(repeater);
	return;
}

void
TextSession::elicit_repeater_deletion(DraftJournal& journal)
{
	journal.clear_repeaters();
	cout << "\nAutomatic recording has been disabled for this transaction.\n"
	     << endl;
	return;
}

void
TextSession::exit_journal_edit_without_saving
(	PersistentJournal& journal
)
{
	journal.ghostify();
	cout << "Saved transaction remains as follows: "
	     << endl << endl << journal << endl;
	return;
}

void
TextSession::exit_journal_edit_saving_changes
(	PersistentJournal& journal
)
{
	journal.save();
	cout << "Saved transaction is now as follows: "
	     << endl << endl << journal << endl;
	return;
}

void
TextSession::elicit_date_amendment
(	OrdinaryJournal& journal
)
{
	cout << "\nEnter new transaction date in the form YYYYMMDDD "
	     << "(or Enter to abort): ";
	optional<gregorian::date> const maybe_date = get_date_from_user(true);	
	if (!maybe_date)
	{
		cout << "Date left unchanged." << endl;
		return;
	}
	assert (maybe_date);
	journal.set_date(value(maybe_date));
	return;
}

void
TextSession::populate_journal_editing_menu_core
(	Menu& menu,
	PersistentJournal& journal
)
{
	typedef shared_ptr<MenuItem const> ItemPtr;
	typedef TextSession PTS;  // For brevity below.

	ItemPtr add_entry_item
	(	new MenuItem
		(	"Add a line",
			bind(bind(&PTS::elicit_entry_insertion, this, _1), ref(journal)),
			false,
			"al"
		)
	);
	menu.push_item(add_entry_item);

	ItemPtr delete_entry_item
	(	new MenuItem
		(	"Delete a line",
			bind(bind(&PTS::elicit_entry_deletion, this, _1), ref(journal)),
			false,
			"dl"
		)
	);
	menu.push_item(delete_entry_item);

	ItemPtr amend_entry_item
	(	new MenuItem
		(	"Edit a line",
			bind(bind(&PTS::elicit_entry_amendment, this, _1), ref(journal)),
			false,
			"el"
		)
	);
	menu.push_item(amend_entry_item);

	ItemPtr amend_comment_item
	(	new MenuItem
		(	"Edit transaction comment",
			bind
			(	bind(&PTS::elicit_comment_amendment, this, _1), ref(journal)
			),
			false,
			"et"
		)
	);
	menu.push_item(amend_comment_item);
	return;
}


void
TextSession::finalize_journal_editing_cycle
(	PersistentJournal& journal,
	Menu& menu,
	bool& exiting,
	bool simple_exit,
	bool journal_type_is_draft
)
{
	// TODO
	// Note messages to user are possibly confusing in the event that the
	// user converts a draft to an ordinary transaction. To
	// exit they must choose either to retain or abandon changes made,
	// but is confusing as to whether the "changes" include the posting
	// of the OrdinaryJournal (they don't).
	string const jnl_type_descr =
		journal_type_is_draft? " draft" : "";
	typedef shared_ptr<MenuItem const> ItemPtr;
	typedef TextSession PTS;  // For brevity below
	ItemPtr delete_journal_item
	(	new MenuItem
		(	"Delete transaction",
			bind
			(	bind(&PTS::elicit_journal_deletion, this, _1),
				ref(journal)
			),
			false,
			"dt"
		)
	);
	menu.push_item(delete_journal_item);
	ItemPtr simple_exit_item = MenuItem::provide_menu_exit();
	ItemPtr exit_without_saving_item
	(	new MenuItem
		(	"Undo any changes to" + jnl_type_descr + " transaction, and exit",
			bind
			(	bind(&PTS::exit_journal_edit_without_saving, this, _1),
				ref(journal)
			),
			false,
			"u"
		)
	);
	ItemPtr exit_with_saving_item
	(	new MenuItem
		(	"Save any changes to" + jnl_type_descr + " transaction, and exit",
			bind
			(	bind(&PTS::exit_journal_edit_saving_changes, this, _1),
				ref(journal)
			),
			false,
			"s"
		)
	);
	if (!simple_exit)
	{
		menu.push_item(exit_without_saving_item);
		if (journal.is_balanced())
		{
			menu.push_item(exit_with_saving_item);
		}
		else
		{
			cout << "Note transaction is unbalanced. Entries must sum to "
				 << "nil before changes can be saved.\n" << endl;
		}
	}
	else
	{
		assert (journal.is_balanced());
		menu.push_item(simple_exit_item);
	}
	menu.present_to_user();	
	ItemPtr const last_choice = menu.last_choice();
	if 
	(	(last_choice == exit_without_saving_item) ||
		(last_choice == exit_with_saving_item) ||
		(last_choice == simple_exit_item) ||
		(last_choice == delete_journal_item)
	)
	{
		exiting = true;
	}
	else assert (!exiting);
	return;
}

	
void
TextSession::elicit_ordinary_journal_from_draft
(	DraftJournal& draft_journal
)
{
	typedef shared_ptr<MenuItem const> ItemPtr;

	OrdinaryJournal ordinary_journal(database_connection());
	ordinary_journal.mimic(draft_journal);

	// TODO There is duplicated code between here and
	// finalize_ordinary_journal
	gregorian::date d = gregorian::day_clock::local_day();
	cout << "Enter transaction date as an eight-digit number of the "
		 << "form YYYYMMDD, or just hit enter for today's date ("
		 << gregorian::to_iso_string(d)
		 << "): ";
	optional<gregorian::date> const date_input = get_date_from_user(true);
	if (date_input) d = value(date_input);
	ordinary_journal.set_date(d);
	
	cout << "Ordinary transaction will be recorded dated "
	     << d
		 << ". Draft transaction will be retained in list. Proceed? (y/n) ";
	string const confirmation = get_constrained_user_input
	(	boost::lambda::_1 == "y" || boost::lambda::_1 == "n",
		"Try again, entering \"y\" to record transaction "
		"or \"n\" to abort: ",
		false
	);
	if (confirmation == "y")
	{
		ordinary_journal.save();
		cout << "\nOrdinary transaction recorded:\n" << endl
		     << ordinary_journal << endl;
		cout << "\nDraft transaction also remains saved:" << endl;
	}
	else
	{
		assert (confirmation == "n");
		cout << "\nTransaction has not been recorded. "
		     << "However draft transaction remains saved: "
			 << endl;
	}

	return;
}

	

	

void
TextSession::conduct_draft_journal_editing(DraftJournal& journal)
{
	typedef shared_ptr<MenuItem const> ItemPtr;
	typedef TextSession PTS;  // For brevity below.

	for 
	(	bool exiting = false, first_time = true;
		!exiting;
		first_time = false
	)
	{
		cout << endl << journal << endl;
		Menu menu;
		populate_journal_editing_menu_core(menu, journal);

		ItemPtr add_repeater_item
		(	new MenuItem
			(	"Add automatic recording cycle",
				bind
				(	bind(&PTS::elicit_repeater_insertion, this, _1), journal
				),
				false,
				"ar"
			)
		);
		menu.push_item(add_repeater_item);

		ItemPtr delete_repeaters_item
		(	new MenuItem
			(	"Disable automatic recording",
				bind(bind(&PTS::elicit_repeater_deletion, this, _1), journal),
				false,
				"dr"
			)
		);
		ItemPtr convert_to_ordinary_journal_item
		(	new MenuItem
			(	"Record as ordinary transaction",
				bind
				(	bind(&PTS::elicit_ordinary_journal_from_draft, this, _1),
					journal
				),
				false,
				"ro"
			)
		);
		if (journal.has_repeaters())
		{
			menu.push_item(delete_repeaters_item);
		}
		else if (journal.is_balanced())
		{
			assert (!journal.has_repeaters());
			menu.push_item(convert_to_ordinary_journal_item);
		}
		finalize_journal_editing_cycle
		(	journal,
			menu,
			exiting,
			first_time,
			true
		);
	}
	return;
}

void
TextSession::conduct_ordinary_journal_editing
(	OrdinaryJournal& journal
)
{
	typedef shared_ptr<MenuItem const> ItemPtr;
	typedef TextSession PTS;
	for
	(	bool exiting = false, first_time = true;
		!exiting;
		first_time = false
	)
	{
		cout << endl << journal << endl;
		Menu menu;
		populate_journal_editing_menu_core(menu, journal);

		ItemPtr amend_date_item
		(	new MenuItem
			(	"Edit journal date",
				bind(bind(&PTS::elicit_date_amendment, this, _1), journal),
				false,
				"ed"
			)
		);
		menu.push_item(amend_date_item);

		finalize_journal_editing_cycle
		(	journal,
			menu,
			exiting,
			first_time,
			false
		);
	}
	return;
}


void
TextSession::display_account_detail()
{
	namespace col = column_creation;
	cout << endl;
	Table<Account> table;
	typedef Table<Account>::ColumnPtr ColumnPtr;
	ColumnPtr const name_column(col::create_account_name_column());
	table.push_column(name_column);
	ColumnPtr const type_column(col::create_account_type_column());
	table.push_column(type_column);
	ColumnPtr const description_column
	(	col::create_account_description_column()
	);
	table.push_column(description_column);
	AccountReader reader(database_connection());
	table.populate(reader.begin(), reader.end());
	cout << endl << table << endl;
	return;
}


void
TextSession::conduct_account_editing()
{
	cout << "\nEnter name of account or category to "
	     << "edit (or just hit Enter to abort): ";
	string const account_name = elicit_existing_account_name(true);
	if (account_name.empty())
	{
		cout << "Procedure aborted.\n" << endl;
		return;
	}
	assert (!account_name.empty());
	Account account(database_connection(), std8_to_bstring(account_name));

	cout << "Enter new name (or Enter to leave unchanged): ";
	string const new_name = elicit_unused_account_name(true);
	if (!new_name.empty()) account.set_name(std8_to_bstring(new_name));

	cout << "Enter new description (or Enter to leave unchanged): ";
	string const new_description = get_user_input();
	if (!new_description.empty())
	{
		account.set_description(std8_to_bstring(new_description));
	}
	account.save();
	cout << "Changes have been saved:" << endl;
	if (!new_name.empty())
	{
		cout << "New name: " << new_name << endl;
	}
	if (!new_description.empty())
	{
		cout << "New description: " << new_description << endl;
	}
	cout << endl;
	return;
}


void
TextSession::conduct_reconciliation()
{
	// TODO This is a giant mess and needs refactoring
	Account account(database_connection());
	for (bool input_is_valid = false; !input_is_valid; )
	{
		cout << "Enter name of asset, liability or equity account "
			 << "(or leave blank to abort): ";
		string const account_name = elicit_existing_account_name(true);
		if (account_name.empty())
		{
			cout << "Reconciliation aborted.\n" << endl;
			return;
		}
		assert (!account_name.empty());
		account = Account(database_connection(), account_name);
		if (!is_balance_sheet_account(account))
		{
			cout << "Only asset, liability or equity accounts can "
				 << "be reconciled. "
				 << "Please try again."
				 << endl;
			assert (!input_is_valid);
		}
		else
		{
			input_is_valid = true;
		}
	}
	assert
	(	account.account_super_type() ==
		account_super_type::balance_sheet
	);
	cout << "Enter statement opening date as an 8-digit number of the form"
		 << " YYYYMMDD: ";
	gregorian::date const opening_date = value(get_date_from_user());
	gregorian::date closing_date;	
	for (bool input_is_valid = false; !input_is_valid; )
	{
		cout << "Enter statement closing date as an 8-digit number of the "
			 << "form YYYYMMDD: ";
		closing_date = value(get_date_from_user());
		if (closing_date >= opening_date)
		{
			input_is_valid = true;
		}
		else
		{
			assert (closing_date < opening_date);
			cout << "Closing date cannot be earlier than opening date. "
				 << "Please try again: ";
			assert (input_is_valid == false);
		}
	}


	for (bool exiting = false; !exiting; )
	{
		// TODO Factor out common code shared between here and
		// display_ordinary_actual_entries().
		
		Decimal opening_balance(0, 0);
		Decimal reconciled_balance(0, 0);
		ActualOrdinaryEntryReader reader(database_connection());
		vector<Entry> table_vec;
		typedef ActualOrdinaryEntryReader::const_iterator ReaderIt;
		ReaderIt const end = reader.end();
		ReaderIt it = reader.begin();
		// Examine entries prior to the statement opening date
		for ( ; (it != end) && (it->date() < opening_date); ++it)
		{
			if (it->account() == account)
			{
				Decimal const amount = it->amount();
				if (it->is_reconciled())
				{
					reconciled_balance += amount;
					opening_balance += amount;
				}
				else
				{
					table_vec.push_back(*it);
				}
			}
		}

		// Examine entries between the opening and closing dates
		for ( ; (it != end) && (it->date() <= closing_date); ++it)
		{
			if (it->account() == account)
			{
				table_vec.push_back(*it);
				Decimal const amount = it->amount();
				if (it->is_reconciled()) reconciled_balance += amount;
			}
		}
		
		// Configure Table
		Table<Entry> table;
		typedef Table<Entry>::ColumnPtr ColumnPtr;
		namespace col = column_creation;
		ColumnPtr const date_column
		(	col::create_entry_ordinary_journal_date_column()
		);
		table.push_column(date_column);
		ColumnPtr const journal_id_column
		(	col::create_entry_ordinary_journal_id_column()
		);
		table.push_column(journal_id_column);
		ColumnPtr const id_column(col::create_entry_id_column());
		table.push_column(id_column);
		ColumnPtr const comment_column(col::create_entry_comment_column());
		table.push_column(comment_column);
#		ifdef PHATBOOKS_EXPOSE_COMMODITY
			ColumnPtr const commodity_column
			(	col::create_entry_commodity_abbreviation_column()
			);
			table.push_column(commodity_column);	
#		endif
		ColumnPtr const amount_column(col::create_entry_amount_column());
		table.push_column(amount_column);
		ColumnPtr const running_total_column
		(	col::create_entry_running_total_amount_column
			(	opening_balance
			)
		);
		table.push_column(running_total_column);
		ColumnPtr const reconciliation_status_column
		(	col::create_entry_reconciliation_status_column()
		);
		table.push_column(reconciliation_status_column);
		table.populate(table_vec.begin(), table_vec.end());

		// Display Table
		cout << endl << table << endl;

		// Summarise totals
		cout << "\nReconciled balance at "
			 << closing_date << ": "
			 << finformat_std8(reconciled_balance) << endl;
		cout << endl;

		// Get user input on which to reconcile
		cout << "Enter \"a\" to mark all entries as reconciled, "
		     << "\"u\" to unmark all, or"
		     << " a series of entry IDs - separated by spaces - to toggle "
			 << "the reconciliation status of selected entries (or just hit "
			 << "Enter to return to the previous menu) :"
			 << endl;
		for (bool input_is_valid = false; !input_is_valid; )
		{
			typedef vector<Entry>::iterator EntryIt;
			string const input = get_user_input();
			// TODO I should use for_each algorithm here.
			if (input == "a")
			{
				DatabaseTransaction transaction(database_connection());	
				for
				(	EntryIt tvit = table_vec.begin(), tvend = table_vec.end();
					tvit != tvend;
					++tvit
				)
				{
					tvit->set_whether_reconciled(true);
					tvit->save();
				}
				input_is_valid = true;
				transaction.commit();
			}
			else if (input == "u")
			{
				DatabaseTransaction transaction(database_connection());	
				for
				(	EntryIt tvit = table_vec.begin(), tvend = table_vec.end();
					tvit != tvend;
					++tvit
				)
				{
					tvit->set_whether_reconciled(false);
					tvit->save();
				}
				input_is_valid = true;
				transaction.commit();
			}
			else if (input.empty())
			{
				input_is_valid = true;
				exiting = true;
			}
			else
			{
				regex const target_regex("^([1-9][0-9]*[ ]?)+$");
				if (!regex_match(input, target_regex))
				{
					cout << "Please try again, entering \"a\", \"u\", or a "
					     << "series of numbers, being entry ids from the table"
						 << " above, separated by spaces:" << endl;
					assert (input_is_valid == false);
				}
				else
				{
					using boost::algorithm::is_space;
					vector<string> id_strings;
					split(id_strings, input, is_space());
					vector<string> invalid_ids;
					DatabaseTransaction transaction(database_connection());	
					for
					(	vector<string>::const_iterator
							isit = id_strings.begin(),
							isend = id_strings.end();
						isit != isend;
						++isit
					)
					{
						try
						{
							Entry::Id const id =
								lexical_cast<Entry::Id>(*isit);
							Entry entry(database_connection(), id);
							vector<Entry>::const_iterator eit =
								table_vec.begin();
							vector<Entry>::const_iterator eend =
								table_vec.end();
							for ( ; (eit != eend) && (eit->id() != id); ++eit)
							{
							}
							if (eit == eend)
							{
								invalid_ids.push_back(*isit);
							}
							else
							{
								if (entry.is_reconciled())
								{
									entry.set_whether_reconciled(false);
								}
								else
								{
									entry.set_whether_reconciled(true);
								}
								entry.save();
							}
						}
						catch (bad_lexical_cast&)
						{
						}
						catch (BadIdentifier&)
						{
							invalid_ids.push_back(*isit);	
						}
					}
					transaction.commit();
					if (!invalid_ids.empty())
					{
						cout << "The following ids did not correspond to "
						     << "entries in the above table, and were "
							 << "ignored:"
							 << endl;
						for
						(	vector<string>::const_iterator invit =
									invalid_ids.begin(),
								invend = invalid_ids.end();
							invit != invend;
							++invit
						)
						{
							cout << *invit << " ";
						}
						cout << endl << endl;
					}
					input_is_valid = true;
				}
			}
		}
	}
	return;
}



void
TextSession::display_journal_from_id()
{
	// The lexical casts are to prevent the insertion of thousands
	// separators in the id numbers
	cout << "Enter the ID of the transaction you want to view ("
	     << lexical_cast<string>(min_journal_id(database_connection()))
		 << "-"
		 << lexical_cast<string>(max_journal_id(database_connection()))
		 << "): ";
	std::string const input = get_constrained_user_input
	(	bind(identifies_existent_journal, &database_connection(), _1),
		"There is no journal with this id. Try again: "
	);
	if (input.empty())
	{
		return;
	}
	// We know this will work
	Journal::Id const id = lexical_cast<Journal::Id>(input);
	if (journal_id_is_draft(database_connection(), id))
	{
		DraftJournal dj(database_connection(), id);
		conduct_draft_journal_editing(dj);
	}
	else
	{
		OrdinaryJournal oj(database_connection(), id);
		conduct_ordinary_journal_editing(oj);
	}
	// TODO In both conduct_..._editing functions,
	// the "save changes" and "exit without saving changes" options are
	// displayed even if the user has not made any changes to the journal.
	// This is potentially confusing for the user.
	return;
}




void
TextSession::display_ordinary_actual_entries()
{
	// TODO There is probably factor-out-able code between this and the
	// Draft/Ordinary/Journal printing methods.
	
	// TODO Could the following procedure result in overflow as it may
	// add all and only the ACTUAL entries for a P&L account? How should we
	// deal with this possibility?
	
	// TODO The implementation here is a bit clumsy and slow.
	// Some possible ways of improving it are as follows.
	//
	// (1) Go and rescue the abandoned "filtering Reader" from svn, and
	// use that. That would probably take care of the speed issue; however,
	// I would then need another mechanism for calculating the opening
	// balance of account - which depends on inspecting every entry up
	// to the start date. (Would another Reader-like thing work here?
	// Yeah but I want to minimize my use of SQL-based solutions.)
	//
	// (2) Make date a field of Entry as well as of
	// Journal. This would solve a lot of problems but
	// at the cost of horrible extra complexity inside the EntryImpl and
	// ProtoJournal classes - to make sure the Entry attributes are
	// synched with the corresponding Journal attributes (date and
	// is_actual).
	//
	// (3) A related issue: is there a way to make a non-template
	// version of Entry<...>::journal()? This feels like it might also
	// solve some problems -- it might make the code to retrieve the date
	// and is_actual attributes of an Entry cleanly enough that we don't
	// need to worry about the other stuff... and see below.
	//
	// (4) Could we do something to bulk-load journals fast at the
	// start of the session. The loading method for journals is
	// really inefficient... NOTE But this seems to have been largely
	// solved by the creation of an index over entry(journal_id).

	optional<Account> maybe_account;
	cout << "Enter account name (or leave blank to show all accounts): ";
	string const account_name = elicit_existing_account_name(true);
	if (!account_name.empty())
	{
		maybe_account = Account(database_connection(), account_name);
	}
	else
	{
		assert (!maybe_account);
	}

	// TODO Factor out process of "getting date range from user" into a
	// separate function.
	cout << "Enter start date as an 8-digit number of the form YYYYMMDD, "
			"or leave blank for no start date: ";
	optional<gregorian::date> const maybe_earliest_date =
		get_date_from_user(true);

	optional<gregorian::date> maybe_latest_date;
	for (bool input_is_valid = false; !input_is_valid; )
	{
		cout << "Enter end date as an 8-digit number of the form YYYYMMDD, "
				"or leave blank for no end date: ";
		maybe_latest_date = get_date_from_user(true);
		if 
		(	!maybe_earliest_date ||
			!maybe_latest_date || 
			(value(maybe_latest_date) >= value(maybe_earliest_date))
		)
		{
			input_is_valid = true;
		}
		else
		{
			assert (maybe_earliest_date);
			assert (maybe_latest_date);
			assert (value(maybe_latest_date) < value(maybe_earliest_date));
			cout << "End date cannot be earlier than start date. "
					"Please try again: ";
			assert (!input_is_valid);
		}
	}
	
	Decimal opening_balance(0, 0);
	bool const filtering_for_account = (maybe_account? true: false);
	if (account_name.empty()) assert (!filtering_for_account);
	Account account(database_connection());
	if (maybe_account) account = value(maybe_account);
	bool const is_balance_sheet =
	(	maybe_account?
		is_balance_sheet_account(value(maybe_account)):
		false
	);
	bool const accumulating_pre_start_date_entries =
	(	filtering_for_account &&
		is_balance_sheet
	);

	ActualOrdinaryEntryReader reader(database_connection());
	ActualOrdinaryEntryReader::const_iterator it = reader.begin();
	ActualOrdinaryEntryReader::const_iterator const end = reader.end();

	// Examine pre-start-date entries
	if (maybe_earliest_date)
	{
		for
		(	gregorian::date const earliest = *maybe_earliest_date;
			(it != end) && (it->date() < earliest);
			++it
		)
		{
			if
			(	accumulating_pre_start_date_entries &&
				(it->account() == account)
			)
			{
				opening_balance += it->amount();
				assert (is_balance_sheet);
			}
		}
	}
	vector<Entry> table_vec;


	// Examine entries later than or equal to the start date
	Decimal closing_balance = opening_balance;
	for ( ; it != end; ++it)
	{
		if 
		(	maybe_latest_date &&
			(it->date() > *maybe_latest_date)
		)
		{
			break;
		}
		if (filtering_for_account)
		{
			if (it->account() == account)
			{
				table_vec.push_back(*it);
				Decimal const amount = it->amount();
				closing_balance += amount;
			}
		}
		else
		{
			assert (!filtering_for_account);
			table_vec.push_back(*it);
		}
	}

	Table<Entry> table;
	typedef Table<Entry>::ColumnPtr ColumnPtr;
	namespace col = column_creation;
	ColumnPtr const date_column
	(	col::create_entry_ordinary_journal_date_column()
	);
	table.push_column(date_column);
	ColumnPtr const journal_id_column
	(	col::create_entry_ordinary_journal_id_column()
	);
	table.push_column(journal_id_column);
	ColumnPtr const id_column(col::create_entry_id_column());
	table.push_column(id_column);
	if (!filtering_for_account)
	{
		ColumnPtr const account_column
		(	col::create_entry_account_name_column()
		);
		table.push_column(account_column);
	}
	ColumnPtr const comment_column(col::create_entry_comment_column());
	table.push_column(comment_column);
#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		ColumnPtr commodity_column
		(	col::create_entry_commodity_abbreviation_column()
		);
		table.push_column(commodity_column);
#	endif
	ColumnPtr const amount_column(col::create_entry_amount_column());
	table.push_column(amount_column);
	if (filtering_for_account)
	{
		assert (maybe_account);
		if (is_balance_sheet)
		{
			ColumnPtr const running_balance_column
			(	col::create_entry_running_total_amount_column(opening_balance)
			);
			table.push_column(running_balance_column);
		}
	}

	table.populate(table_vec.begin(), table_vec.end());
	cout << endl << table << endl;

	if (filtering_for_account)
	{
		assert (maybe_account);
		summarise_balance_movement
		(	value(maybe_account),
			opening_balance,
			closing_balance
		);
	}

	return;
}

#ifdef PHATBOOKS_EXPOSE_COMMODITY
	void
	TextSession::elicit_commodity()
	{
		Commodity commodity(database_connection());

		// Get abbreviation
		cout << "Enter abbreviation for new commodity: ";
		for (bool input_is_valid = false; !input_is_valid; )
		{
			string input = get_user_input();
			if (input.empty())
			{
				cout << "Abbreviation cannot be blank. Please try again: ";
			}
			else if
			(	Commodity::exists_with_abbreviation
				(	database_connection(),
					std8_to_bstring(input)
				)
			)
			{
				cout << "A commodity with this abbreviation already exists. "
					 << "Please try again: ";
			}
			else
			{
				input_is_valid = true;
				commodity.set_abbreviation(std8_to_bstring(input));
			}
		}

		// Get commodity name
		cout << "Enter name for new commodity (cannot be blank): ";
		for (bool input_is_valid = false; !input_is_valid; )
		{
			string input = get_user_input();
			if (input.empty())
			{
				cout << "Name cannot be blank. Please try again: ";
			}
			else if
			(	Commodity::exists_with_name
				(	database_connection(),
					std8_to_bstring(input)
				)
			)
			{
				cout << "A commodity with this name already exists. "
					 << "Please try a different name: ";
			}
			else
			{
				input_is_valid = true;
				commodity.set_name(std8_to_bstring(input));
			}
		}
			
		// Get description 
		cout << "Enter description for new commodity (or hit enter for no "
				"description): ";
		commodity.set_description(std8_to_bstring(get_user_input()));

		// Get precision 
		cout << "Enter precision required for this commodity "
			 << "(a number from 0 to 6, representing the number of decimal "
			 << "places of precision to the right of the decimal point): ";
		for (bool input_is_valid = false; !input_is_valid; )
		{
			string input = get_user_input();
			if (!regex_match(input, regex("^[0123456]$")))
			{
				cout << "Please try again, entering a number from 0 to 6: ";
			}
			else
			{
				commodity.set_precision(lexical_cast<int>(input));
				input_is_valid = true;
			}
		}
		assert (commodity.precision() >= 0 && commodity.precision() <= 6);

		// Get multiplier to base
		cout << "Enter rate by which this commodity should be multiplied in order"
			 << " to convert it to the base commodity for this entity: ";
		commodity.set_multiplier_to_base(value(get_decimal_from_user()));

		// Confirm with user before creating commodity
		cout << endl << "You have proposed to create the following commodity: "
			 << endl << endl
			 << "Abbreviation: " << commodity.abbreviation() << endl
			 << "Name: " << commodity.name() << endl
			 << "Description: " << commodity.description() << endl
			 << "Precision: " << commodity.precision() << endl
			 << "Conversion rate to base: " << commodity.multiplier_to_base()
			 << endl << endl;
		cout << "Proceed with creating this commodity? (y/n) ";
		string const confirmation = get_constrained_user_input
		(	boost::lambda::_1 == "y" || boost::lambda::_1 == "n",
			"Try again, entering \"y\" to create commodity "
			"or \"n\" to abort: ",
			false
		);
		if (confirmation == "n")
		{
			cout << "Commodity not created." << endl;
		}
		else
		{
			assert (confirmation == "y");
			commodity.save();
			cout << "Commodity created." << endl;
		}
		return;
	}
#endif  // PHATBOOKS_EXPOSE_COMMODITY


string
TextSession::elicit_unused_account_name(bool allow_empty_to_escape)
{
	string input;
	for (bool input_is_valid = false; !input_is_valid; )
	{
		input = get_user_input();
		if (input.empty())
		{
			if (allow_empty_to_escape)
			{
				return input;
			}
			else
			{
				cout << "Name cannot be blank. Please try again: ";
			}
		}
		else if
		(	Account::exists(database_connection(), std8_to_bstring(input))
		)
		{
			cout << "An account with this name already exists. "
			     << "Please try again: ";
		}
		else
		{
			input_is_valid = true;
		}
	}
	return input;
}

		



void
TextSession::elicit_account()
{

	Account account(database_connection());

	// Get account name
	cout << "Enter a name for the account: ";
	account.set_name(std8_to_bstring(elicit_unused_account_name()));

#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		// Get commodity abbreviation
		cout << "Enter the abbreviation of the commodity that will be the "
			 << "native commodity of this account: ";
		for (bool input_is_valid = false; !input_is_valid; )
		{
			string input = get_user_input();
			if 
			(	!Commodity::exists_with_abbreviation
				(	database_connection(),
					std8_to_bstring(input)
				)
			)
			{
				cout << "There is no commodity with this abbreviation. Please "
					 << "try again: ";
			}
			else
			{
				input_is_valid = true;
				account.set_commodity
				(	Commodity(database_connection(), std8_to_bstring(input))
				);
			}
		}
#	else
		account.set_commodity
		(	Commodity::default_commodity(database_connection())
		);
#	endif  // PHATBOOKS_EXPOSE_COMMODITY

	// Get account type
	Menu account_type_menu;
	vector<BString> const names = account_type_names();
	for (vector<BString>::size_type i = 0; i != names.size(); ++i)
	{
		shared_ptr<MenuItem> item(new MenuItem(bstring_to_std8(names[i])));
		account_type_menu.push_item(item);
	}
	cout << "What kind of account do you wish to create?" << endl;
	account_type_menu.present_to_user();
	string const account_type_name =
		account_type_menu.last_choice()->banner();
	account.set_account_type(string_to_account_type(account_type_name));


	// Get description 
	cout << "Enter description for new account (or hit enter for no "
	        "description): ";
	account.set_description(std8_to_bstring(get_user_input()));
	
	// Confirm with user before creating account
	cout << endl << "You have proposed to create the following account: "
	     << endl << endl
	     << "Name: " << account.name() << endl
#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		 << "Commodity: " << account.commodity().abbreviation() << endl
#	endif
		 << "Type: " << account_type_name << endl
		 << "Description: " << account.description() << endl
		 << endl;
	cout << "Proceed with creating this account? (y/n) ";
	string const confirmation = get_constrained_user_input
	(	boost::lambda::_1 == "y" || boost::lambda::_1 == "n",
		"Try again, entering \"y\" to create account "
		"or \"n\" to abort: ",
		false
	);
	if (confirmation == "n")
	{
		cout << "Account not created." << endl;
	}
	else
	{
		assert (confirmation == "y");
		account.save();
		cout << "Account created." << endl;
	}
	return;
}



Repeater
TextSession::elicit_repeater()
{
	Repeater repeater(database_connection());
	cout << "\nHow often do you want this transaction to be recorded?"
		 << endl;
	Menu frequency_menu;
	shared_ptr<MenuItem> monthly_day_x
	(	new MenuItem
		(	"Every month, on a given day of the month (except the "
			"29th, 30th or 31st)"
		)
	);
	frequency_menu.push_item(monthly_day_x);
	shared_ptr<MenuItem> monthly_day_last
	(	new MenuItem("Every month, on the last day of the month")
	);
	frequency_menu.push_item(monthly_day_last);
	shared_ptr<MenuItem> N_monthly_day_x
	(	new MenuItem
		(	"Every N months, on a given day of the month (except the "
			"29th, 30th or 31st)"
		)
	);
	frequency_menu.push_item(N_monthly_day_x);
	shared_ptr<MenuItem> N_monthly_day_last
	(	new MenuItem("Every N months, on the last day of the month")
	);
	frequency_menu.push_item(N_monthly_day_last);
	shared_ptr<MenuItem> weekly(new MenuItem("Every week"));
	frequency_menu.push_item(weekly);
	shared_ptr<MenuItem> N_weekly(new MenuItem("Every N weeks"));
	frequency_menu.push_item(N_weekly);
	shared_ptr<MenuItem> daily(new MenuItem("Every day"));
	frequency_menu.push_item(daily);
	shared_ptr<MenuItem> N_daily(new MenuItem("Every N days"));
	frequency_menu.push_item(N_daily);
	frequency_menu.present_to_user();
	shared_ptr<MenuItem const> const choice =
		frequency_menu.last_choice();

		

	// Determine frequency step type
	optional<interval_type::IntervalType> step_type;
	if (choice == monthly_day_x || choice == N_monthly_day_x)
	{
		step_type = interval_type::months;
	}
	else if
	(	choice == monthly_day_last ||
		choice == N_monthly_day_last
	)
	{
		step_type = interval_type::month_ends;
	}
	else if (choice == weekly || choice == N_weekly)
	{
		step_type = interval_type::weeks;
	}
	else if (choice == daily || choice == N_daily)
	{
		step_type = interval_type::days;
	}
	assert (step_type);

	// Determine frequency number of steps
	optional<int> num_steps;
	if
	(	choice == monthly_day_x ||
		choice == monthly_day_last ||
		choice == weekly ||
		choice == daily
	)
	{
		num_steps = 1;
	}
	else
	{
		string unit_description;
		if (choice == N_weekly) unit_description = "week";
		else if (choice == N_daily) unit_description = "day";
		else unit_description = "month";
		cout << "Enter the number of "
			 << unit_description
			 << "s between each occurrence of the transaction: ";
		for (bool is_valid = false; !is_valid; )
		{
			string const input = get_user_input();
			if (!regex_match(input, regex("^[0-9]+$")))
			{
			}
			else if (input == "0")
			{
			}
			else
			{
				try
				{
					num_steps = lexical_cast<int>(input);
					is_valid = true;
				}
				catch (bad_lexical_cast&)
				{
				}
			}
			if (!is_valid)
			{
				cout << "Try again, entering a number greater "
						"than 0: ";
			}
		}
	}
	// Set frequency
	assert (step_type);
	assert (num_steps);
	repeater.set_frequency(Frequency(value(num_steps), value(step_type)));

	// Determine next posting date
	cout << "Enter the first date on which the transaction will occur"
		 << ", as an eight-digit number of the form YYYYMMDD (or just"
		 << " hit enter for today's date): ";
	optional<gregorian::date> const d = get_date_from_user(true);
	if (d) repeater.set_next_date(*d);
	else repeater.set_next_date(gregorian::day_clock::local_day());
	return repeater;
}


string
TextSession::dialogue_phrase
(	TransactionType transaction_type,
	PhraseType phrase_type
)
{
	typedef string Vocab[num_phrase_types];
	typedef Vocab VocabMap[num_transaction_types];
	static VocabMap const dictionary =
	{	{	"account from which money was spent",
			"spent",
			"expenditure category",
			"expenditure category",
			"categories"
		},
		{	"account into which funds were deposited",
			"earned",
			"revenue category",
			"revenue category",
			"categories"
		},
		{	"destination account",
			"transferred",
			"source account",
			"source account",
			"source accounts"
		},
		{	"envelope you wish to top up",
			"to transfer",
			"envelope from which to source funds",
			"source envelope",
			"source envelopes"
		},
		{	"account or category",
			"",
			"account or category",
			"account or category",
			"accounts / categories"
		}
	};
	int const transaction_index = static_cast<int>(transaction_type);
	int const phrase_index = static_cast<int>(phrase_type);
	return dictionary[transaction_index][phrase_index];
}




bool
TextSession::account_is_valid
(	TransactionType transaction_type,
	TransactionPhase transaction_phase,
	Account const& account,
	string& validity_description
)
{
	int const transaction_type_index = static_cast<int>(transaction_type);
	int const transaction_phase_index = static_cast<int>(transaction_phase);
	typedef AccountValidator ValidatorArray[num_transaction_phases];
	typedef ValidatorArray ValidatorMatrix[num_transaction_types];
	ValidatorMatrix const validator_matrix =
	{	{ is_asset_or_liability, is_expense },
		{ is_asset_or_liability, is_revenue },
		{ is_balance_sheet_account, is_balance_sheet_account },
		{ is_pl_account, is_pl_account },
		{ is_not_pure_envelope, is_not_pure_envelope }
	};
	AccountValidator const validate =
		validator_matrix[transaction_type_index][transaction_phase_index];
	validity_description = validator_description(validate);
	return validate(account);
}


optional<Account>
TextSession::elicit_valid_account
(	TransactionType transaction_type,
	TransactionPhase transaction_phase,
	bool allow_empty_to_escape
)
{
	while (true)
	{
		optional<Account> ret;
		string const account_name = elicit_existing_account_name(true);
		if (allow_empty_to_escape && account_name.empty())
		{
			return ret;
		}
		else if (account_name.empty())
		{
			assert (!allow_empty_to_escape);
			cout << "There is no account with this name. Please try again: ";
			continue;
		}
		assert (!account_name.empty());
		Account const account(database_connection(), account_name);
		string guide;
		if
		(	!account_is_valid
			(	transaction_type,
				transaction_phase,
				account,
				guide
			)
		)
		{
			cout << account.name() << " is not a valid " << guide << ". ";
			cout << "Please try again: ";
		}
		else
		{
			ret = account;
			return ret;
		}
	}
}

	
	
TextSession::TransactionType
TextSession::elicit_transaction_type()
{
	Menu menu;
	shared_ptr<MenuItem> expenditure_selection
	(	new MenuItem
		(	"Expenditure transaction",
			MenuItem::do_nothing,
			false,
			"e"
		)
	);
	expenditure_selection->set_hiding_condition
	(	bind
		(	account_types_missing_for_expenditure_transaction,
			ref(database_connection())
		)
	);
	menu.push_item(expenditure_selection);
	shared_ptr<MenuItem> revenue_selection
	(	new MenuItem
		(	"Revenue transaction",
			MenuItem::do_nothing,
			false,
			"r"
		)
	);
	revenue_selection->set_hiding_condition
	(	bind
		(	account_types_missing_for_revenue_transaction,
			ref(database_connection())
		)
	);
	menu.push_item(revenue_selection);
	shared_ptr<MenuItem> balance_sheet_selection
	(	new MenuItem
		(	"Transfer between asset, liability or equity accounts",
			MenuItem::do_nothing,
			false,
			"a"
		)
	);
	balance_sheet_selection->set_hiding_condition
	(	bind
		(	no_balance_sheet_accounts_saved,
			ref(database_connection())
		)
	);
	menu.push_item(balance_sheet_selection);
	shared_ptr<MenuItem> envelope_selection
	(	new MenuItem
		(	"Transfer between budgeting envelopes",
			MenuItem::do_nothing,
			false,
			"b"
		)
	);
	envelope_selection->set_hiding_condition
	(	bind(no_pl_accounts_saved, ref(database_connection()))
	);
	menu.push_item(envelope_selection);
	menu.present_to_user();
	shared_ptr<MenuItem const> const selection = menu.last_choice();
	TransactionType const ret =
	(	selection == expenditure_selection? expenditure_transaction:
		selection == revenue_selection? revenue_transaction:
		selection == balance_sheet_selection? balance_sheet_transaction:
		envelope_transaction
	);
	return ret;
}




void
TextSession::elicit_primary_entries
(	ProtoJournal& journal,
	TransactionType transaction_type
)
{
	// TODO Enable multiple (split) primary entries
	Entry entry(database_connection());
	cout << "Enter name of "
	     << dialogue_phrase(transaction_type, account_prompt)
		 << ": ";
	entry.set_account
	(	value(elicit_valid_account(transaction_type, primary_phase))
	);
	Commodity const commodity = entry.account().commodity();
	Decimal const amount = elicit_constrained_amount
	(	commodity,
		dialogue_phrase(transaction_type, amount_prompt)
	);
	bool const sign_needs_changing =
	(	transaction_type == expenditure_transaction ||
		transaction_type == envelope_transaction
	);
	entry.set_amount(sign_needs_changing? -amount: amount);
	cout << "Comment for this line (or Enter for no comment): ";
	entry.set_comment(std8_to_bstring(get_user_input()));
	entry.set_whether_reconciled(false);
	journal.push_entry(entry);
	return;
}


void
TextSession::elicit_secondary_entries
(	ProtoJournal& journal,
	TransactionType transaction_type
)
{
	Entry secondary_entry(database_connection());
	bool const sign_needs_changing =
	(	transaction_type != expenditure_transaction &&
		transaction_type != envelope_transaction
	);
	Decimal const initial_friendly_balance =
	(	sign_needs_changing?
		journal.balance():
		-journal.balance()
	);
	cout << "Enter name of "
	     << dialogue_phrase(transaction_type, secondary_account_prompt)
		 << ", or leave blank to split between multiple "
		 << dialogue_phrase(transaction_type, secondary_account_prompt_plural)
		 << ": ";
	Commodity const primary_commodity =
		journal.entries().begin()->account().commodity();
	optional<Account> const account_opt =
		elicit_valid_account(transaction_type, secondary_phase, true);
	if (!account_opt)
	{
		// We have multiple secondary entries (split transaction)
		cout << endl;
		Decimal unmatched_amount = initial_friendly_balance;
		Decimal const zero(0, 0);
		for (int i = 1; unmatched_amount != zero; ++i)
		{
			Entry current_entry(database_connection());
			cout << "Enter name of "
			     << dialogue_phrase
				 	(	transaction_type,
						secondary_account_prompt_simple
					)
				<< " no. " << i << ": ";
			current_entry.set_account
			(	value(elicit_valid_account(transaction_type, secondary_phase))
			);
			Commodity const current_commodity
				= current_entry.account().commodity();
#			ifndef PHATBOOKS_EXPOSE_COMMODITY
				assert (current_commodity == primary_commodity);
#			endif
			if (current_commodity != primary_commodity)
			{
				// TODO Deal with this!
				throw std::logic_error("Mismatched commodities.");
			}
			// TODO Remove code duplication between here and
			// elicit_primary_entry
			Decimal current_entry_amount(0, 0);
			for (bool input_is_valid = false; !input_is_valid; )
			{
				cout << "Amount remaining to split: "
#				ifdef PHATBOOKS_EXPOSE_COMMODITY
				     << current_commodity.abbreviation() << " "
#				endif
					 << " " << unmatched_amount << endl;
				cout << "Enter amount for this line: ";
				current_entry_amount = value(get_decimal_from_user());
				Decimal::places_type const entered_precision =
					current_entry_amount.places();
				try
				{
					current_entry_amount = jewel::round
					(	current_entry_amount, current_commodity.precision()
					);
					if (current_entry_amount.places() < entered_precision)
					{
						cout << "Amount rounded to " << current_entry_amount
						     << ". " << endl;
					}
					if (current_entry_amount > unmatched_amount)
					{
						cout << "You have entered an amount greater than "
						     << "what remains to be split. Please try again."
							 << endl;
					}
					else
					{
						unmatched_amount -= current_entry_amount;
						input_is_valid = true;
					}
				}
				catch (DecimalRangeException&)
				{
					cout << "The number you entered cannot be safely"
					     << " rounded to the required precision of "
						 << current_commodity.precision()
						 << " decimal places. Please try again."
						 << endl;
					assert (!input_is_valid);
				}
			}
			current_entry.set_amount
			(	sign_needs_changing?
				-current_entry_amount:
				current_entry_amount
			);
			cout << "Comment for this line (or Enter for no comment): ";
			current_entry.set_comment(std8_to_bstring(get_user_input()));
			current_entry.set_whether_reconciled(false);
			journal.push_entry(current_entry);
			cout << endl;
		}
	}
	else
	{
		assert (account_opt);
		secondary_entry.set_account(value(account_opt));
		// WARNING if secondary account is in a different currency then we
		// need to deal with this here somehow.
		Commodity const secondary_commodity =
			secondary_entry.account().commodity();
#		ifndef PHATBOOKS_EXPOSE_COMMODITY
			assert (secondary_commodity == primary_commodity);
#		endif
		if
		(	secondary_commodity != primary_commodity
		)
		{
			// TODO Deal with this.
			throw std::logic_error("Mismatched commodities.");
		}
		cout << "Line specific comment (or Enter for no comment): ";
		secondary_entry.set_comment(std8_to_bstring(get_user_input()));
		secondary_entry.set_amount(-journal.balance());
		secondary_entry.set_whether_reconciled(false);
		journal.push_entry(secondary_entry);
	}
	return;
}


void
TextSession::finalize_ordinary_journal(OrdinaryJournal& journal)
{
	gregorian::date d = gregorian::day_clock::local_day();
	cout << "Enter transaction date as an eight-digit number of the "
		 << "form YYYYMMDD, or just hit enter for today's date ("
		 << gregorian::to_iso_string(d)
		 << "): ";

	optional<gregorian::date> const date_input = get_date_from_user(true);
	if (date_input) d = value(date_input);
	journal.set_date(d);
	journal.save();
	cout << "\nTransaction recorded:" << endl << endl
		 << journal << endl;
	return;
}


void
TextSession::finalize_draft_journal
(	DraftJournal& journal,
	bool autopost
)
{
	cout << "Enter a name for the "
	     << (autopost? "recurring": "draft")
		 << " transaction: ";
	for (bool is_valid = false; is_valid != true; )
	{
		string name = get_user_input();
		if (name.empty())
		{
			cout << "Name cannot be blank. Please try again: ";
		}
		else if
		(	DraftJournal::exists(database_connection(), std8_to_bstring(name))
		)
		{
			cout << "A draft or recurring transaction has already "
				 << "been saved under this name. Please enter a "
				 << "another name: ";
		}
		else
		{
			journal.set_name(std8_to_bstring(name));
			is_valid = true;
		}
	}
	// TODO Allow for multiple repeaters
	if (autopost)
	{
		Repeater repeater = elicit_repeater();
		journal.push_repeater(repeater);
	}
	journal.save();
	cout << "Draft journal has been saved:" << endl << endl
		 << journal << endl;
	return;
}


void
TextSession::finalize_journal(ProtoJournal& journal)
{
	cout << endl << journal << endl << endl;
	shared_ptr<MenuItem> post
	(	new MenuItem
		(	"Record transaction",
			MenuItem::do_nothing,
			false,
			"r"
		)
	);
	shared_ptr<MenuItem> save_draft
	(	new MenuItem
		(	"Save as a draft to return and complete later",
			MenuItem::do_nothing,
			false,
			"sd"
		)
	);
	shared_ptr<MenuItem> save_recurring
	(	new MenuItem
		(	"Save as a recurring transaction",
			MenuItem::do_nothing,
			false,
			"sr"
		)
	);
	shared_ptr<MenuItem> abandon
	(	new MenuItem
		(	"Abandon transaction without saving",
			MenuItem::do_nothing,
			false,
			"a"
		)
	);
	Menu journal_action_menu;
	journal_action_menu.push_item(post);
	journal_action_menu.push_item(save_draft);
	journal_action_menu.push_item(save_recurring);
	journal_action_menu.push_item(abandon);
	journal_action_menu.present_to_user();
	shared_ptr<MenuItem const> journal_action =
		journal_action_menu.last_choice();

	if (journal_action == post)
	{
		OrdinaryJournal oj(database_connection());
		oj.mimic(journal);
		finalize_ordinary_journal(oj);
	}
	else if (journal_action == save_draft)
	{
		DraftJournal dj(database_connection());
		dj.mimic(journal);
		finalize_draft_journal(dj, false);
	}
	else if (journal_action == save_recurring)
	{
		DraftJournal dj(database_connection());
		dj.mimic(journal);
		finalize_draft_journal(dj, true);
	}
	else if (journal_action == abandon)
	{
		cout << "\nTransaction has not been posted or saved." << endl;
	}
	else
	{
		// Execution should not reach here.
		assert (false);
	}
	return;

	// TODO
	// Before posting, we need to ensure
	// the entries will not overflow the account balances.
}



void
TextSession::elicit_journal()
{
	ProtoJournal journal;
	TransactionType const transaction_type = elicit_transaction_type();
	journal.set_whether_actual(transaction_type != envelope_transaction);
	cout << "Enter a comment describing the transaction (or Enter to "
	        "leave blank): ";
	journal.set_comment(std8_to_bstring(get_user_input()));
	cout << endl;
	elicit_primary_entries(journal, transaction_type);
	cout << endl;
	elicit_secondary_entries(journal, transaction_type);
	cout << endl << "Completed transaction is as follows:" << endl;
	finalize_journal(journal);
	return;
}



void TextSession::notify_autoposts
(	shared_ptr<list<OrdinaryJournal> > journals
) const
{
	if (!journals->empty())
	{
		cout << "The following journals have been posted automatically since "
			 << "the last session:"
			 << endl << endl;
		for
		(	list<OrdinaryJournal>::const_iterator it = journals->begin(),
				end = journals->end();
			it != end;
			++it
		)
		{
			cout << *it << endl << endl;
		}
	}
	return;
}




void TextSession::display_balance_sheet()
{
	// TODO Locale reversion is not exception-safe here.
	// TODO Code is duplicated between here and display_envelopes().
	BalanceSheetAccountReader const bs_reader(database_connection());
	locale const orig_loc = cout.getloc();
	cout.imbue(locale(""));
	cout << endl << endl;
	cout << "BALANCE SHEET: " << endl << endl;
	print_bs_account_reader(bs_reader);
	cout.imbue(orig_loc);
	return;
}

void TextSession::display_envelopes()
{
	// TODO Locale reversion is not exception-safe here.
	PLAccountReader pl_reader(database_connection());
	locale const orig_loc = cout.getloc();
	cout.imbue(locale(""));
	cout << endl << endl;
	cout << "ENVELOPES: " << endl << endl;
	print_pl_account_reader(pl_reader, database_connection());
	cout.imbue(orig_loc);
	return;
}



void TextSession::wrap_up()
{
	return;
}


}  // namespace tui
}  // namespace phatbooks
