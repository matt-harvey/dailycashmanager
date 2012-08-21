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
#include "consolixx.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
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
using consolixx::TextSession;
using jewel::Decimal;
using jewel::DecimalFromStringException;
using jewel::DecimalRangeException;
using sqloxx::InvalidFilename;
using sqloxx::SQLiteException;
using boost::bind;
using boost::function;
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


PhatbooksTextSession::~PhatbooksTextSession()
{
	wrap_up();
}


bool has_three_letters(string const& s)
{
	return s.size() == 3;
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
	// We need the user's input to populate all these variables
	string commodity_abbreviation;
	string commodity_name;
	string commodity_description;
	int commodity_precision;
	Decimal commodity_multiplier_to_base("0");

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
		(
			m_database_connection->has_commodity_with_abbreviation(input)
		)
		{
			cout << "A commodity with this abbreviation already exists. "
			     << "Please try again: ";
		}
		else
		{
			input_is_valid = true;
			commodity_abbreviation = input;
		}
	}

	// Get commodity name
	cout << "Enter name for new commodity (or enter for no name): ";
	for (bool input_is_valid = false; !input_is_valid; )
	{
		string input = get_user_input();
		if (m_database_connection->has_commodity_named(input))
		{
			cout << "A commodity with this name already exists. "
			     << "Please try a different name, or hit Enter for "
				 << "no name: ";
		}
		else
		{
			input_is_valid = true;
			if (input.empty())
			{
				cout << "Commodity will be nameless." << endl;
				assert (input == "");
			}
			commodity_name = input;
		}
	}
		
	// Get description 
	cout << "Enter description for new commodity (or hit enter for no "
	        "description): ";
	commodity_description = get_user_input();

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
			commodity_precision = lexical_cast<int>(input);
			input_is_valid = true;
		}
	}
	assert (commodity_precision >= 0 && commodity_precision <= 6);

	// Get multiplier to base
	cout << "Enter rate by which this commodity should be multiplied in order"
	     << " to convert it to the base commodity for this entity: ";
	for (bool input_is_valid = false; !input_is_valid; )
	{
		try
		{
			commodity_multiplier_to_base = Decimal(get_user_input());
			input_is_valid = true;
		}
		catch (DecimalFromStringException&)
		{
			assert (commodity_multiplier_to_base == Decimal("0"));
			assert (!input_is_valid);
			cout << "Please try again, entering a decimal number, "
			     << "(e.g. \"1.343\"): ";
		}
		catch (DecimalRangeException&)
		{
			assert (commodity_multiplier_to_base == Decimal("0"));
			assert (!input_is_valid);
			cout << "This conversion rate is too large or too small to be "
			     << "safely handled. Please try again: ";
		}
	}

	// Confirm with user before creating commodity
	cout << endl << "You have proposed to create the following commodity: "
	     << endl << endl
	     << "Abbreviation: " << commodity_abbreviation << endl
		 << "Name: " << commodity_name << endl
		 << "Description: " << commodity_description << endl
		 << "Precision: " << commodity_precision << endl
		 << "Conversion rate to base: " << commodity_multiplier_to_base
		 << endl << endl;
	cout << "Proceed with creating this commodity? (y/n) ";
	string confirmation = get_constrained_user_input
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
		Commodity comm
		(	commodity_abbreviation,
			commodity_name,
			commodity_description,
			commodity_precision,
			commodity_multiplier_to_base
		);
		m_database_connection->store(comm);
		cout << "Commodity created." << endl;
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
	// We need the user's input to populate all these variables
	string account_name;
	string commodity_abbreviation;
	string account_type_name;
	Account::AccountType account_type;
	string account_description;

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
			account_name = input;
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
			commodity_abbreviation = input;
		}
	}

	// Get account type
	Menu account_type_menu;
	typedef map<string, Account::AccountType> map_type;
	map_type dict = Account::account_type_dictionary();
	for (map_type::const_iterator it = dict.begin(); it != dict.end(); ++it)
	{
		shared_ptr<MenuItem> item(new MenuItem(it->first, do_nothing, false));
		account_type_menu.add_item(item);
	};
	cout << "What kind of account do you wish to create?" << endl;
	account_type_menu.present_to_user();
	account_type_name = account_type_menu.last_choice()->banner();
	account_type = dict[account_type_name];

	// Get description 
	cout << "Enter description for new account (or hit enter for no "
	        "description): ";
	account_description = get_user_input();
	
	// Confirm with user before creating account
	cout << endl << "You have proposed to create the following account: "
	     << endl << endl
	     << "Name: " << account_name << endl
		 << "Commodity: " << commodity_abbreviation << endl
		 << "Type: " << account_type_name << endl
		 << "Description: " << account_description << endl
		 << endl << endl;
	cout << "Proceed with creating this account? (y/n) ";
	string confirmation = get_constrained_user_input
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
		Account acc
		(	account_name,
			commodity_abbreviation,
			account_type,
			account_description
		);
		m_database_connection->store(acc);
		cout << "Account created." << endl;
	}
	return;


}


void PhatbooksTextSession::wrap_up()
{
	return;
}


}  // namespace phatbooks
