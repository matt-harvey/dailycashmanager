
/** \file phatbooks_database_connection.cpp
 *
 * \brief Source file pertaining to PhatbooksDatabaseConnection class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */





#include "account.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "repeater.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <boost/bimap.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>  // from import_from_nap
#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <cassert>
#include <fstream>  // for import_from_nap
#include <iostream>
#include <list>
#include <map>  // for import_from_nap
#include <stdexcept>
#include <string>
#include <vector> // for import_from_nap


using boost::bimap;
using boost::numeric_cast;
using boost::shared_ptr;
using jewel::Decimal;
using sqloxx::DatabaseConnection;
using sqloxx::SQLStatement;
using sqloxx::SQLiteException;
using std::endl;
using std::list;
using std::map;  // for import_from_nap
using std::runtime_error;
using std::string;
using std::vector;  // for import_from_nap



namespace phatbooks
{


PhatbooksDatabaseConnection::PhatbooksDatabaseConnection():
  DatabaseConnection()
{
}


bool
PhatbooksDatabaseConnection::has_account_named(string const& p_name)
{
	SQLStatement statement
	(	*this,
		"select name from accounts where name = :p"
	);
	statement.bind(":p", p_name);
	return statement.step();
}


bimap<Account::AccountType, string>
PhatbooksDatabaseConnection::account_types()
{
	typedef bimap<Account::AccountType, string> return_type;
	return_type ret;
	SQLStatement statement
	(	*this,
		"select account_type_id, name from account_types order "
		"by account_type_id"
	);
	while (statement.step())
	{
		Account::AccountType acctype = static_cast<Account::AccountType>
		(	statement.extract<IdType>(0)
		);
		string accname = statement.extract<string>(1);
		ret.insert(return_type::value_type(acctype, accname));
	}
	return ret;
}



bool
PhatbooksDatabaseConnection::has_commodity_with_abbreviation
(	string const& p_abbreviation
)
{
	SQLStatement statement
	(	*this,
		"select abbreviation from commodities where abbreviation = :p"
	);
	statement.bind(":p", p_abbreviation);
	return statement.step();
}

bool
PhatbooksDatabaseConnection::has_commodity_named(string const& p_name)
{
	SQLStatement statement
	(	*this,
		"select name from commodities where name = :p"
	);
	statement.bind(":p", p_name);
	return statement.step();
}


void
PhatbooksDatabaseConnection::setup()
{
	if (setup_has_occurred())
	{
		return;
	}
	assert (!setup_has_occurred());
	execute_sql("begin transaction");
	setup_boolean_table();
	Commodity::setup_tables(*this);
	Account::setup_tables(*this);
	Journal::setup_tables(*this);
	Repeater::setup_tables(*this);
	Entry::setup_tables(*this);
	mark_setup_as_having_occurred();
	execute_sql("end transaction");
	assert (setup_has_occurred());
	return;
}

namespace
{
	string const setup_flag = "setup_flag_996149162";
}

void
PhatbooksDatabaseConnection::mark_setup_as_having_occurred()
{
	execute_sql("create table " + setup_flag + "(dummy_column);");
	return;
}



bool
PhatbooksDatabaseConnection::setup_has_occurred()
{
	try
	{
		execute_sql("select * from " + setup_flag);
		return true;
	}
	catch (SQLiteException&)
	{
		return false;
	}
}


// Anonymous namespace for helper functions and constants used
// in import_from_nap
namespace
{
	bool is_daggger(char c)
	{
		return c == '|';
	}

	bool is_colon(char c)
	{
		return c == ':';
	}

	// Takes a string and returns a vector of strings split by
	// the dagger character.
	vector<string> cells(string const& row_str)
	{
		vector<string> ret;
		return boost::algorithm::split(ret, row_str, is_dagger);
	}

}  // End anonymous namespace

void
PhatbooksDatabaseConnection::import_from_nap
(	boost::filesystem::path const& directory
)
{
	if
	(	!boost::filesystem::is_directory(boost::filesystem::status(directory))
	)
	{
		throw std::logic_error
		(	"Non-directory passed to import_from_nap; import_from_nap should "
			"be passed the name of the directory containing the required csv "
			"files."
		);
	}

	string const account_csv_name = "accountshelf.csv";
	string const draft_entry_csv_name = "draftentryshelf.csv";
	string const draft_journal_csv_name = "draftjournalshelf.csv";
	string const entry_csv_name = "entryshelf.csv";
	string const journal_csv_name = "journalshelf.csv";

	boost::filesystem::directory_iterator it(directory);
	int check = 0;
	while (it != boost::filesystem::directory_iterator())
	{
		string const s = it->filename();
		if (s == account_csv_name) check += 1;
		else if (s == draft_entry_csv_name) check += 10;
		else if (s == draft_journal_csv_name) check += 100;
		else if (s == entry_csv_name) check += 1000;
		else if (s == journal_csv_name) check += 10000;
	}
	if (check != 11111)
	{
		throw std::runtime_error
		(	"The csv files required by import_from_nap  are not all present."
		);
	}
	string const file_sep = "/";
	
	// Insert the sole commodity
	Commodity aud(*this);
	aud.set_abbreviation("AUD");
	aud.set_name("Australian dollars");
	aud.set_description("");
	aud.set_precision(2);
	aud.set_multiplier_to_base(Decimal("1"));
	aud.save_new();

	// Read accounts
	std::ifstream account_csv
	(	(directory.string() + file_sep + account_csv_name).c_str()
	);
	string account_row;
	while (getline(account_csv, account_row))
	{
		vector<string> const account_cells = cells(account_row);
		Account account(*this);
		switch (lexical_cast<int>(account_cells[0][1]))
		{
		case 1:  // ready asset or...
		case 3:  // investment asset
			account.set_account_type(Account::asset);
			break;
		case 2:  // ready liability or... 
		case 4:  // investment liability
			account.set_account_type(Account::liability);
			break;
		case 5:
			account.set_account_type(Account::equity);
			break;
		case 6:
			account.set_account_type(Account::revenue);
			break;
		case 7:
			account.set_account_type(Account::expense);
			break;
		default:
			throw std::runtime_error("Unrecognised account type.");
		}
		account.set_name(account_cells[1]);
		account.set_commodity_abbreviation(aud.abbreviation());
		account.set_description(account_cells[2]);
		account.save_new();
	}
		
	// Read draft journals ************************************
	
	std::ifstream draft_journal_csv
	(	(directory.string() + file_sep + draft_journal_csv_name).c_str()
	);
	string draft_journal_row;

	typedef vector< shared_ptr<Journal> > JournalVec;

	// We need a map to tell us the meaning of each string representation
	// of a repeater interval type.
	map<string, Repeater::IntervalType> interval_type_map;
	interval_type_map["day"] = Repeater::days;
	interval_type_map["week"] = Repeater::weeks;
	interval_type_map["month"] = Repeater::months;
	interval_type map["end_of_month"] = Repeater::month_ends;

	// We will store Journal instances in this vector temporarily, and
	// run through later to ensure order preserved
	JournalVec draft_journal_vec;

	// We will store a map from the draft journal names in the csv, to
	// Journal instances in memory. Shortly this will enable us to associate
	// draft entries with Journal instances.
	map< string, shared_ptr<Journal> > draft_journal_map;

	// We will store a map from the draft journal names in the csv, to
	// Journal::Id values. This will enable us to remember to PROSPECTIVE id
	// of each Journal, so we can associate each draft entry with the correct
	// journal based on its name.
	map< string, Journal::Id> draft_journal_id_map;

	// Now to actually read the draft journals.
	Journal::Id draft_journal_id = 0;
	while (getline(draft_journal_csv, draft_journal_row))
	{
		++draft_journal_id;

		// Split the csv row into cells
		vector<string> const draft_journal_cells = cells(draft_journal_row);
		shared_ptr<Journal> draft_journal(new Journal(*this));
		journal->set_date(null_date());
		journal->set_comment("");
		string const draft_journal_name = draft_journal_cells[0];
		draft_journal_map[draft_journal_name] = journal;
		draft_journal_id_map[draft_journal_name] = draft_journal_id;
		draft_journal_vec.push_back(journal);
		
		// Now examine the repeater_list cell, which needs to be further
		// split. (The N. A. P. csv design is no normalized database. It
		// was a quick and dirty hack.)
		string const raw_rep_str = draft_journal_cells[1];
		vector<string> rep_str_vec;
		boost::algorithm::split(rep_str_vec, raw_rep_str, is_colon);

		// Now we examine each repeater in the repeater list, create a
		// Repeater instance, and add that to the Journal instance
		// representing the current draft journal.
		for (vector<string>::size_type i = 0; i != rep_str_vec.size(); ++i)
		{
			string const repeater_str = rep_str_vec[i];
			vector<string> repeater_fields;
			boost::algorithm::split
			(	repeater_fields,
				repeater_str,
				boost::algorithm::is_space()
			);
			string const next_date_str = repeater_fields[0];
			string const interval_type_str = repeater_fields[1];
			string const units_str = repeater_fields[2];

			shared_ptr<Repeater> repeater(new Repeater(*this));
			repeater->set_interval_type(interval_type_map[interval_type_str]);
			repeater->set_interval_units(lexical_cast<int>(units_str));
			repeater->set_next_date
			(	boost::gregorian::date_from_iso_string(next_date_str)
			);
			repeater->set_journal_id(draft_journal_id);
			draft_journal->add_repeater(repeater);
		}
	}

	// Read draft entries *******************************************

	std::ifstream draft_entry_csv
	(	(directory.string() + file_sep + draft_entry_csv_name).c_str()
	);
	string draft_entry_row;
	while (getline(draft_entry_csv, draft_entry_row))
	{
		vector<string> const draft_entry_cells = cells(draft_entry_row);
		shared_ptr<Entry> draft_entry(new Entry(*this));
		string const draft_journal_name = draft_entry_cells[0];
		string const comment = draft_entry_cells[2];
		string const account_name = draft_entry_cells[3];
		Decimal act_impact(draft_entry_cells[4]);
		Decimal bud_impact(draft_entry_cells[5]);
		bool const is_actual = (bud_impact == Decimal("0")? true: false);
		draft_entry->set_journal_id(draft_journal_id_map[draft_journal_name]);
		draft_entry->set_account_name(account_name);
		draft_entry->set_comment(comment);
		draft_entry->set_amount(is_actual? act_impact: -bud_impact);	
		draft_journal_map[draft_journal_name]->set_whether_actual(is_actual);
		draft_journal_map[draft_journal_name]->add_entry(entry);

		// WARNING We do need to store the draft journal name in the
		// draft journal - just as soon as Journal class or a related
		// class provides this facility...
	}
	

	// Save the Journal instances corresponding to draft journals into the
	// database.
	for
	(	JournalVec::iterator it = draft_journal_vec.begin();
		it != draft_journal_vec.end();
		++it
	)
	{
		// REMEMBER Verify that the journal_id is as anticipated in
		// draft_journal_id_map.
		(*it)->save_new();
	}

	// ...
	// WARNING implementation incomplete

	// Read entries
	std::ifstream entry_csv
	(	(directory.string() + file_sep + entry_csv_name).c_str()
	);
	// ...
	// WARNING implementation incomplete
	
	// Read journals
	std::ifstream journal_csv
	(	(directory.string() + file_sep + journal_csv_name).c_str()
	);
	// ...
	// WARNING implementation incomplete

	// ...
	// WARNING implementation incomplete
	
	return;
}
			

	


}  // namespace phatbooks
