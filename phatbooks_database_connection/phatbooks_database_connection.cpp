
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
#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <cassert>
#include <fstream>  // for import_from_nap
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>


using boost::bimap;
using boost::numeric_cast;
using boost::shared_ptr;
using jewel::Decimal;
using sqloxx::DatabaseConnection;
using sqloxx::SQLStatement;
using sqloxx::SQLiteException;
using std::endl;
using std::list;
using std::runtime_error;
using std::string;



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

	// Read accounts
	std::ifstream account_csv
	(	(directory.string() + file_sep + account_csv_name).c_str()
	);
	// ...
	// WARNING implementation incomplete
	
	// Read draft entries
	std::ifstream draft_entry_csv
	(	(directory.string() + file_sep + draft_entry_csv_name).c_str()
	);
	// ...
	// WARNING implementation incomplete
	
	// Read draft journals
	std::ifstream draft_journal_csv
	(	(directory.string() + file_sep + draft_journal_csv_name).c_str()
	);
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
