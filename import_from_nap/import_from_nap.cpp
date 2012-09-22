#include "import_from_nap.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

using boost::lexical_cast;
using boost::shared_ptr;
using jewel::Decimal;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;


namespace phatbooks
{



// Anonymous namespace for helper functions and constants used
// in import_from_nap
namespace
{
	
	template <char target>
	bool is_char(char tested_char)
	{
		return tested_char == target;
	}

	// Takes a string and returns a vector of strings split by
	// the character passed as template parameter.
	template <char separator>
	vector<string> cells(string const& row_str)
	{
		vector<string> ret;
		return boost::algorithm::split(ret, row_str, is_char<separator>);
	}

}  // End anonymous namespace




void import_from_nap
(	shared_ptr<PhatbooksDatabaseConnection> database_connection,
	boost::filesystem::path const& directory
)
{
	JEWEL_DEBUG_LOG << "\tChecking filepath is a directory..." << endl;
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

	int check = 0;
	JEWEL_DEBUG_LOG << "\tChecking that CSV files are present..." << endl;
	for
	(	boost::filesystem::directory_iterator it(directory);
		it != boost::filesystem::directory_iterator();
		++it
	)
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
	JEWEL_DEBUG_LOG << "\tCSV files confirmed present." << endl;
	
	// Insert the sole commodity
	JEWEL_DEBUG_LOG << "\tInserting AUD into commodities table..." << endl;
	Commodity aud(database_connection);
	aud.set_abbreviation("AUD");
	aud.set_name("Australian dollars");
	aud.set_description("");
	aud.set_precision(2);
	aud.set_multiplier_to_base(Decimal("1"));
	aud.save_new();
	JEWEL_DEBUG_LOG << "\tAUD inserted." << endl;

	// Read accounts
	JEWEL_DEBUG_LOG << "\tReading accounts from CSV..." << endl;
	string const file_sep = "/";
	std::ifstream account_csv
	(	(directory.string() + file_sep + account_csv_name).c_str()
	);
	string account_row;
	while (getline(account_csv, account_row))
	{
		vector<string> const account_cells = cells<'|'>(account_row);
		Account account(database_connection);

		// The second character of the first field contains a number
		// from 1 to 7, that is sufficient to identify the account type.
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
		JEWEL_DEBUG_LOG << "Inserting account to database..." << endl;
		account.save_new();
		JEWEL_DEBUG_LOG << "Account " << account.name()
		                << " inserted." << endl;
	}
		
	// Read draft journals ************************************
	
	JEWEL_DEBUG_LOG << "Reading draft journals from CSV..." << endl;
	std::ifstream draft_journal_csv
	(	(directory.string() + file_sep + draft_journal_csv_name).c_str()
	);
	string draft_journal_row;

	typedef vector< shared_ptr<DraftJournal> > JournalVec;

	// We make a map to tell us the meaning of each string representation
	// (in the csv) of a repeater interval type, in terms of the enumerations
	// in Repeater::IntervalType.
	map<string, Repeater::IntervalType> interval_type_map;
	interval_type_map["day"] = Repeater::days;
	interval_type_map["week"] = Repeater::weeks;
	interval_type_map["month"] = Repeater::months;
	interval_type_map["end_of_month"] = Repeater::month_ends;

	// We will store Journal instances in this vector temporarily, and
	// run through later to ensure order is preserved. (The order they are
	// inserted into the database will thereby correspond with predicted
	// id.)
	JournalVec draft_journal_vec;

	// We will store a map from the draft journal names in the csv, to
	// Journal instances in memory. Shortly this will enable us to associate
	// draft entries with Journal instances.
	map< string, shared_ptr<DraftJournal> > draft_journal_map;

	// We will store a map from the draft journal names in the csv, to
	// Journal::Id values. This will enable us to remember the PROSPECTIVE id
	// of each Journal, so we can associate each draft entry with the correct
	// journal based on its name.
	map< string, Journal::Id> draft_journal_id_map;

	// Now to actually read the draft journals.
	Journal::Id draft_journal_id = 0;
	while (getline(draft_journal_csv, draft_journal_row))
	{
		++draft_journal_id;

		// Split the csv row into cells
		vector<string> const draft_journal_cells =
			cells<'|'>(draft_journal_row);
		shared_ptr<DraftJournal> draft_journal(new DraftJournal(database_connection));
		draft_journal->set_comment("");
		string const draft_journal_name = draft_journal_cells[0];
		draft_journal->set_name(draft_journal_name);
		draft_journal_map[draft_journal_name] = draft_journal;
		draft_journal_id_map[draft_journal_name] = draft_journal_id;
		draft_journal_vec.push_back(draft_journal);
		
		// Now examine the repeater_list cell, which needs to be further
		// split. (The N. A. P. csv design is no normalized database. It
		// was a quick and dirty hack.)
		string const raw_rep_str = draft_journal_cells[1];
		vector<string> rep_str_vec = cells<':'>(raw_rep_str);

		// Now we examine each repeater in the repeater list, create a
		// Repeater instance, and add that to the DraftJournal instance
		// representing the current draft journal.
		for (vector<string>::size_type i = 0; i != rep_str_vec.size(); ++i)
		{
			string const repeater_str = rep_str_vec[i];
			vector<string> repeater_fields = cells<' '>(repeater_str);
			string const next_date_str = repeater_fields[0];
			string const interval_type_str = repeater_fields[1];
			string const units_str = repeater_fields[2];

			shared_ptr<Repeater> repeater(new Repeater(database_connection));
			repeater->set_interval_type(interval_type_map[interval_type_str]);
			repeater->set_interval_units(lexical_cast<int>(units_str));
			repeater->set_next_date
			(	boost::gregorian::date_from_iso_string(next_date_str)
			);
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
		vector<string> const draft_entry_cells = cells<'|'>(draft_entry_row);
		shared_ptr<Entry> draft_entry(new Entry(database_connection));
		string const draft_journal_name = draft_entry_cells[0];
		string const comment = draft_entry_cells[2];
		string const account_name = draft_entry_cells[3];
		Decimal act_impact(draft_entry_cells[4]);
		Decimal bud_impact(draft_entry_cells[5]);
		// WARNING We need to handle the case where a single Entry has both
		// a non-zero bud_impact and a non-zero act_impact. Given this is
		// just a one-off hack, it may be easier just to manipulate the
		// csv before importing it.
		bool is_actual = true;
		if (bud_impact != Decimal("0"))
		{
			if (act_impact != Decimal("0"))
			{
				throw std::runtime_error
				(	"Import cannot handle entries that have both actual and "
					"budget impacts."
				);
			}
			else
			{
				is_actual = false;
			}
		}
		draft_entry->set_account_name(account_name);
		draft_entry->set_comment(comment);
		draft_entry->set_amount(is_actual? act_impact: -bud_impact);	
		draft_journal_map[draft_journal_name]->set_whether_actual(is_actual);
		draft_journal_map[draft_journal_name]->add_entry(draft_entry);
	}
	
	// Save the Journal instances corresponding to draft journals into the
	// database.
	for
	(	JournalVec::iterator jvit = draft_journal_vec.begin();
		jvit != draft_journal_vec.end();
		++jvit
	)
	{
		// WARNING Verify that the journal_id is as anticipated in
		// draft_journal_id_map.
		(*jvit)->save_new();
	}

	/*
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
	**/
	
	return;
}
			

}  // namespace phatbooks





