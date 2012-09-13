#include "phatbooks_text_session.hpp"


/** \file phatbooks_text_session.cpp
 *
 * \brief Source file for text/console user interface code for Phatbooks.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "consolixx/consolixx.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <boost/bimap.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <map>
#include <string>

using consolixx::get_user_input;
using consolixx::get_constrained_user_input;
using consolixx::get_decimal_from_user;
using consolixx::TextSession;
using jewel::Decimal;
using jewel::DecimalRangeException;
using sqloxx::DatabaseConnection;
using sqloxx::InvalidFilename;
using sqloxx::SQLiteException;
using boost::bimap;
using boost::bind;
using boost::lexical_cast;
using boost::shared_ptr;
using boost::regex;
using boost::regex_match;
using std::cout;
using std::endl;
using std::map;
using std::string;



namespace phatbooks
{

string const PhatbooksTextSession::s_application_name = "Phatbooks";

PhatbooksTextSession::PhatbooksTextSession():
	m_main_menu(new Menu),
	m_database_connection(new PhatbooksDatabaseConnection)
{
	// Set up all the Menu objects.

	shared_ptr<MenuItem> elicit_commodity_item
	(	new MenuItem
		(	"New commodity",
			bind(&PhatbooksTextSession::elicit_commodity, this)
		)
	);
	m_main_menu->add_item(elicit_commodity_item);

	shared_ptr<MenuItem> elicit_account_item
	(	new MenuItem
		(	"New account",
			bind(&PhatbooksTextSession::elicit_account, this)
		)
	);
	m_main_menu->add_item(elicit_account_item);

	shared_ptr<MenuItem> elicit_journal_item
	(	new MenuItem
		(	"New transaction",
			bind(&PhatbooksTextSession::elicit_journal, this)
		)
	);
	m_main_menu->add_item(elicit_journal_item);

	shared_ptr<MenuItem> quit_item
	(	new MenuItem
		(	"Quit",
			bind(&PhatbooksTextSession::wrap_up, this),
			false,
			"x"
		)
	);
	m_main_menu->add_item(quit_item);
}


string
PhatbooksTextSession::elicit_existing_account_name()
{
	 while (true)
	 {
		string input = get_user_input();
	 	if (m_database_connection->has_account_named(input))
		{
			return input;
		}
		cout << "There is no account named " << input
		     << ". Please try again: ";
	}
}


PhatbooksTextSession::~PhatbooksTextSession()
{
	wrap_up();
}


int PhatbooksTextSession::run(string const& filename)
{
	if (filename.empty())
	{
		throw InvalidFilename("Filename is empty string.");
	}
	boost::filesystem::path filepath(filename);
	if (!boost::filesystem::exists(boost::filesystem::status(filepath)))
	{
		cout << "File does not exist. "
		     << "Create file \"" << filename << "\"? (y/n): ";
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
		m_database_connection->open(filepath);
	}
	catch (SQLiteException&)
	{
		cout << "Could not open file \"" << filename << "\"." << endl;
		return 1;
	}

	cout << "Welcome to " << s_application_name << "!" << endl;

	m_database_connection->setup();
	m_main_menu->present_to_user();	
	return 0;
}


void PhatbooksTextSession::elicit_commodity()
{
	Commodity commodity(m_database_connection);

	// Get abbreviation
	cout << "Enter abbreviation for new commodity: ";
	for (bool input_is_valid = false; !input_is_valid; )
	{
		string input = get_user_input();
		if (input.empty())
		{
			cout << "Abbreviation cannot be empty string. Please try again: ";
		}
		else if
		(	m_database_connection->has_commodity_with_abbreviation(input)
		)
		{
			cout << "A commodity with this abbreviation already exists. "
			     << "Please try again: ";
		}
		else
		{
			input_is_valid = true;
			commodity.set_abbreviation(input);
		}
	}

	// Get commodity name
	cout << "Enter name for new commodity (or enter for no name): ";
	for (bool input_is_valid = false; !input_is_valid; )
	{
		string input = get_user_input();
		if (input.empty())
		{
			cout << "Name cannot be blank. Please try again: ";
		}
		else if (m_database_connection->has_commodity_named(input))
		{
			cout << "A commodity with this name already exists. "
			     << "Please try a different name: ";
		}
		else
		{
			input_is_valid = true;
			commodity.set_name(input);
		}
	}
		
	// Get description 
	cout << "Enter description for new commodity (or hit enter for no "
	        "description): ";
	commodity.set_description(get_user_input());

	// Get precision 
	cout << "Enter precision required for this commodity "
	     << "(a number from 0 to 6, representing the number of decimal "
		 << "places of precision to the right of the decimal point): ";
	for (bool input_is_valid = false; !input_is_valid; )
	{
		string input = get_user_input();
		if (!regex_match(input, regex("[0123456]")))
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
	commodity.set_multiplier_to_base(get_decimal_from_user());

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
		commodity.save_new();
		cout << "Commodity created." << endl;
		commodity.set_description("Testing...");
		commodity.save_existing();
	}
	return;
}


// Bleugh! Needed below.
namespace
{
	void do_nothing()
	{
		return;
	}
}


void PhatbooksTextSession::elicit_account()
{
	Account account(m_database_connection);

	// Get account name
	cout << "Enter a name for the account: ";
	for (bool input_is_valid = false; !input_is_valid; )
	{
		string input = get_user_input();
		if (input.empty())
		{
			cout << "Name cannot be empty string. Please try again: ";
		}
		else if (m_database_connection->has_account_named(input))
		{
			cout << "An account with this name already exists. "
			     << "Please try again: ";
		}
		else
		{
			input_is_valid = true;
			account.set_name(input);
		}
	}

	// Get commodity abbreviation
	cout << "Enter the abbreviation of the commodity that will be the "
	     << "native commodity of this account: ";
	for (bool input_is_valid = false; !input_is_valid; )
	{
		string input = get_user_input();
		if (!m_database_connection->has_commodity_with_abbreviation(input))
		{
			cout << "There is no commodity with this abbreviation. Please "
			     << "try again: ";
		}
		else
		{
			input_is_valid = true;
			account.set_commodity_abbreviation(input);
		}
	}

	// Get account type
	Menu account_type_menu;
	typedef bimap<Account::AccountType, string> bimap_type;
	bimap_type account_type_info = m_database_connection->account_types();
	for
	(	bimap_type::iterator it = account_type_info.begin();
		it != account_type_info.end();
		++it
	)
	{
		shared_ptr<MenuItem> item
		(	new MenuItem(it->right, do_nothing, false)
		);
		account_type_menu.add_item(item);
	};
	cout << "What kind of account do you wish to create?" << endl;
	account_type_menu.present_to_user();
	string const account_type_name =
		account_type_menu.last_choice()->banner();
	account.set_account_type(account_type_info.right.at(account_type_name));


	// Get description 
	cout << "Enter description for new account (or hit enter for no "
	        "description): ";
	account.set_description(get_user_input());
	
	// Confirm with user before creating account
	cout << endl << "You have proposed to create the following account: "
	     << endl << endl
	     << "Name: " << account.name() << endl
		 << "Commodity: " << account.commodity_abbreviation() << endl
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
		account.save_new();
		cout << "Account created." << endl;
	}
	return;
}


void PhatbooksTextSession::elicit_journal()
{
	// We need the user's input to populate all these variables
	bool journal_is_actual;
	string journal_comment;
	DateType journal_date;
	Journal journal;

	// Find out what kind of journal this is going to be
	// Do we want to have them all be general journals?
	
	Menu transaction_menu;

	shared_ptr<MenuItem> expenditure_selection
	(	new MenuItem
		(	"Expenditure transaction",
			do_nothing,
			false
		)
	);
	transaction_menu.add_item(expenditure_selection);
	
	shared_ptr<MenuItem> revenue_selection
	(	new MenuItem
		(	"Revenue transaction",
			do_nothing,
			false
		)
	);
	transaction_menu.add_item(revenue_selection);

	shared_ptr<MenuItem> balance_sheet_transfer_selection
	(	new MenuItem
		(	"Transfer between assets or liabilities",
			do_nothing,
			false
		)
	);
	transaction_menu.add_item(balance_sheet_transfer_selection);
	
	shared_ptr<MenuItem> envelope_transaction_selection
	(	new MenuItem
		(	"Transfer between budgeting envelopes",
			do_nothing,
			false
		)
	);
	transaction_menu.add_item(envelope_transaction_selection);

	transaction_menu.present_to_user();
	shared_ptr<MenuItem const> const transaction_type =
		transaction_menu.last_choice();

	// Determine whether journal is actual
	journal.set_whether_actual
	(	transaction_type != envelope_transaction_selection
	);

	// Get journal comment
	cout << "Enter a comment describing the transaction (or Enter to "
	        "leave blank): ";
	journal.set_comment(get_user_input());

	// Set certain "prompt words"
	string account_prompt;
	string amount_prompt;
	string secondary_account_prompt;
	bool primary_sign_needs_changing;
	if (transaction_type == expenditure_selection)
	{
		account_prompt = "account from which money was spent";
		amount_prompt = "spent";
		secondary_account_prompt = "expenditure category";
		primary_sign_needs_changing = true;
	}
	else if (transaction_type == revenue_selection)
	{
		account_prompt = "account into which funds were deposited";
		amount_prompt = "earned";
		secondary_account_prompt = "revenue category";
		primary_sign_needs_changing = false;
	}
	else if (transaction_type == balance_sheet_transfer_selection)
	{
		account_prompt = "destination account";
		amount_prompt = "transferred";
		secondary_account_prompt = "source account";
		primary_sign_needs_changing = false;
	}
	else
	{
		assert (transaction_type == envelope_transaction_selection);
		account_prompt = "envelope you wish to top up";
		amount_prompt = "to transfer";
		secondary_account_prompt = "envelope from which to source funds";
		primary_sign_needs_changing = false;
	}
	
	// Get primary entry account
	cout << "Enter name of " << account_prompt << ": ";
	string primary_entry_account_name = elicit_existing_account_name();

	// Get primary entry amount
	Account primary_entry_account
	(	m_database_connection,
		primary_entry_account_name
	);
	Commodity primary_commodity
	(	m_database_connection,
		primary_entry_account.commodity_abbreviation()
	);
	Decimal primary_entry_amount;
	for (bool input_is_valid = false; !input_is_valid; )
	{
		cout << "Enter amount " << amount_prompt << " (in units of "
			 << primary_commodity.abbreviation() << "): ";
		primary_entry_amount = get_decimal_from_user();
		Decimal::places_type const initial_precision =
			primary_entry_amount.places();
		try
		{
			primary_entry_amount = jewel::round
			(	primary_entry_amount, primary_commodity.precision()
			);
			input_is_valid = true;
			if (primary_entry_amount.places() < initial_precision)
			{
				cout << "Amount rounded to " << primary_entry_amount
				     << "." << endl;
			}
		}
		catch (DecimalRangeException&)
		{
			cout << "The number you entered cannot be safely"
				 << " rounded to the precision required for "
				 << primary_commodity.abbreviation() << ". Please try again."
				 << endl;
			assert (!input_is_valid);
		}
	}
	// Primary entry amount must be changed to the appropriate sign
	// WARNING In theory this might throw.
	Decimal normalized_primary_entry_amount =
		primary_sign_needs_changing?
		primary_entry_amount * Decimal("-1"):
		primary_entry_amount;

	// Get primary entry comment
	cout << "Line specific comment (or Enter for no comment): ";
	string primary_entry_comment = get_user_input();

	// Create entry and add to journal
	shared_ptr<Entry> primary_entry
	(	new Entry
		(	primary_entry_account_name,
			primary_entry_comment,
			normalized_primary_entry_amount
		)
	);
	journal.add_entry(primary_entry);

	// Get other account and comment
	cout << "Enter name of " << secondary_account_prompt << ": ";
	string secondary_entry_account_name = elicit_existing_account_name();
	// WARNING if secondary account is in a different currency then we need to
	// deal with this here somehow.
 
	Account secondary_entry_account
	(	m_database_connection,
		secondary_entry_account_name
	);
	Commodity secondary_commodity
	(	m_database_connection,
		secondary_entry_account.commodity_abbreviation()
	);
	if
	(	secondary_commodity.abbreviation() != primary_commodity.abbreviation()
	)
	{
		JEWEL_DEBUG_LOG << "Here's where we're supposed to respond to "
		                << "diverse commodities..." << endl;
	}

	cout << "Line specific comment (or Enter for no comment): ";
	string secondary_entry_comment = get_user_input();

	

	shared_ptr<Entry> secondary_entry
	(	new Entry
		(	secondary_entry_account_name,
			secondary_entry_comment,
			-normalized_primary_entry_amount
		)
	);
	journal.add_entry(secondary_entry);

		

	// WARNING
	// We need to implement split transactions

	// WARNING
	// We also need to insert Entry objects, and ensure the journal either
	// balances or is to be a draft journal. If it's draft, we need to create
	// Repeater objects if required. Before posting, we need to ensure
	// the entries will not overflow the account balances.
	// Note there are complications when a single Journal involves multiple
	// commodities.
	
	// WARNING
	// Don't forget to get the date!

	m_database_connection->save(journal);
	return;
}


void PhatbooksTextSession::wrap_up()
{
	return;
}


}  // namespace phatbooks
