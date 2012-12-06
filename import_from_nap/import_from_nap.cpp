#include "import_from_nap.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "interval_type.hpp"
#include "journal.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include "sqloxx/database_transaction.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

using boost::unordered_map;
using boost::lexical_cast;
using boost::shared_ptr;
using boost::unordered_set;
using jewel::Decimal;
using sqloxx::DatabaseTransaction;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// TEMP for speed testing
#include <boost/date_time/posix_time/posix_time.hpp>
// END TEMP










namespace phatbooks
{



// Anonymous namespace for helper functions and constants used
// in import_from_nap
namespace
{
	
	template <char target>
	inline
	bool is_char(char tested_char)
	{
		return tested_char == target;
	}

	// Takes a string and returns a vector of strings split by
	// the character passed as template parameter.
	template <char separator>
	inline
	void easy_split(string const& row_str, vector<string>& vec)
	{
		boost::algorithm::split(vec, row_str, is_char<separator>);
		return;
	}

	Decimal const decimal_zero("0");


}  // End anonymous namespace



void import_from_nap
(	shared_ptr<PhatbooksDatabaseConnection> database_connection,
	boost::filesystem::path const& directory
)
{
	DatabaseTransaction transaction(*database_connection);

	boost::posix_time::ptime const start_time =
		boost::posix_time::second_clock::local_time();
	cout << "Import commences at: " << start_time << endl;

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
	string const ordinary_entry_csv_name = "entryshelf.csv";
	string const ordinary_journal_csv_name = "journalshelf.csv";

	int check = 0;
	for
	(	boost::filesystem::directory_iterator it(directory);
		it != boost::filesystem::directory_iterator();
		++it
	)
	{
		string const s = it->path().filename().string();
		if (s == account_csv_name) check += 1;
		else if (s == draft_entry_csv_name) check += 10;
		else if (s == draft_journal_csv_name) check += 100;
		else if (s == ordinary_entry_csv_name) check += 1000;
		else if (s == ordinary_journal_csv_name) check += 10000;
	}
	if (check != 11111)
	{
		throw std::runtime_error
		(	"The csv files required by import_from_nap  are not all present."
		);
	}
	
	// Insert the sole commodity
	Commodity aud(*database_connection);
	aud.set_abbreviation("AUD");
	aud.set_name("Australian dollars");
	aud.set_description("");
	aud.set_precision(2);
	aud.set_multiplier_to_base(Decimal("1"));
	aud.save();

	string const file_sep = "/";

	// Read accounts
	std::ifstream account_csv
	(	(directory.string() + file_sep + account_csv_name).c_str()
	);
	string account_row;
	vector<string> account_cells;
	while (getline(account_csv, account_row))
	{
		easy_split<'|'>(account_row, account_cells);
		Account account(*database_connection);

		// The second character of the first field contains a number
		// from 1 to 7, that is sufficient to identify the account type.
		switch (account_cells[0][1])
		{
		case '1':  // ready asset or...
		case '3':  // investment asset
			account.set_account_type(account_type::asset);
			break;
		case '2':  // ready liability or... 
		case '4':  // investment liability
			account.set_account_type(account_type::liability);
			break;
		case '5':
			account.set_account_type(account_type::equity);
			break;
		case '6':
			account.set_account_type(account_type::revenue);
			break;
		case '7':
			account.set_account_type(account_type::expense);
			break;
		default:
			throw std::runtime_error("Unrecognised account type.");
		}
		string const name = account_cells[1];
		account.set_name(name);
		account.set_commodity(aud);
		account.set_description(account_cells[2]);
		account.save();
	}
	


	typedef vector< shared_ptr<OrdinaryJournal> > OrdinaryJournalVec;
	typedef vector< shared_ptr<DraftJournal> > DraftJournalVec;

	// Maps to record, if we determine that a particular Journal
	// (whether ordinary or draft) is actual (true) or budget (false).
	// This will enable us to track which ones have mixed budget and
	// actuals entries (which must be handled separately).
	unordered_map< shared_ptr<OrdinaryJournal>, bool> oj_actual_v_budget_determinations;
	unordered_map< shared_ptr<DraftJournal>, bool> dj_actual_v_budget_determinations;

	// Set to hold Journal instances which are problematic for Phatbooks
	unordered_set< shared_ptr<OrdinaryJournal> > problematic_ordinary_journals;
	unordered_set< shared_ptr<DraftJournal> > problematic_draft_journals;
	// Map from problematic ordinary journals to old ordinary journal ids
	unordered_map< shared_ptr<OrdinaryJournal>, int > problematic_ordinary_journal_map;
	// Map from problematic draft journals to old draft journal names
	unordered_map< shared_ptr<DraftJournal>, string > problematic_draft_journal_map;





	// Read draft journals ************************************
	
	std::ifstream draft_journal_csv
	(	(directory.string() + file_sep + draft_journal_csv_name).c_str()
	);
	string draft_journal_row;

	// We make a map to tell us the meaning of each string representation
	// (in the csv) of a repeater interval type, in terms of the enumerations
	// in Repeater::IntervalType.
	unordered_map<string, Repeater::IntervalType> interval_type_map;
	interval_type_map["day"] = interval_type::days;
	interval_type_map["week"] = interval_type::weeks;
	interval_type_map["month"] = interval_type::months;
	interval_type_map["end_of_month"] = interval_type::month_ends;

	// We will store DraftJournal instances in this vector temporarily, and
	// run through later to ensure order is preserved. (The order they are
	// inserted into the database will thereby correspond with predicted
	// id.)
	DraftJournalVec draft_journal_vec;

	// We will store a map from the draft journal names in the csv, to
	// DraftJournal instances in memory. Shortly this will enable us to
	// associate draft entries with DraftJournal instances.
	unordered_map< string, shared_ptr<DraftJournal> > draft_journal_map;

	// We will store a map from the draft journal names in the csv, to
	// Journal::Id values. This will enable us to remember the PROSPECTIVE id
	// of each DraftJournal, so we can associate each draft entry with the
	// correct journal based on its name.
	unordered_map< string, Journal::Id> draft_journal_id_map;

	// Now to actually read the draft journals.
	Journal::Id draft_journal_id = 0;
	vector<string> draft_journal_cells;
	while (getline(draft_journal_csv, draft_journal_row))
	{
		++draft_journal_id;

		// Split the csv row into cells
		easy_split<'|'>(draft_journal_row, draft_journal_cells);
		shared_ptr<DraftJournal> draft_journal
		(	new DraftJournal(*database_connection)
		);
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
		vector<string> rep_str_vec;
		easy_split<':'>(raw_rep_str, rep_str_vec);

		// Now we examine each repeater in the repeater list, create a
		// Repeater instance, and add that to the DraftJournal instance
		// representing the current draft journal.
		vector<string> repeater_fields;
		for (vector<string>::size_type i = 0; i != rep_str_vec.size(); ++i)
		{
			string const repeater_str = rep_str_vec[i];
			easy_split<' '>(repeater_str, repeater_fields);
			string const next_date_str = repeater_fields[0];
			string const interval_type_str = repeater_fields[1];
			string const units_str = repeater_fields[2];

			Repeater repeater(*database_connection);
			repeater.set_interval_type(interval_type_map[interval_type_str]);
			repeater.set_interval_units
			(	lexical_cast<int>(units_str.c_str())
			);
			repeater.set_next_date
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
	vector<string> draft_entry_cells;
	while (getline(draft_entry_csv, draft_entry_row))
	{
		easy_split<'|'>(draft_entry_row, draft_entry_cells);
		Entry draft_entry(*database_connection);
		string const draft_journal_name = draft_entry_cells[0];
		string const comment = draft_entry_cells[2];
		string const account_name = draft_entry_cells[3];
		Decimal act_impact(draft_entry_cells[4]);
		Decimal bud_impact(draft_entry_cells[5]);
		bool is_actual = (bud_impact == decimal_zero);
		draft_entry.set_account(Account(*database_connection, account_name));
		draft_entry.set_comment(comment);
		draft_entry.set_amount(is_actual? act_impact: -bud_impact);	
		shared_ptr<DraftJournal> draft_journal =
			draft_journal_map[draft_journal_name];
		draft_entry.set_whether_reconciled(false);
		draft_journal->add_entry(draft_entry);
		if
		(	dj_actual_v_budget_determinations.find(draft_journal) ==
			dj_actual_v_budget_determinations.end()
		)
		{
			// This journal has yet to be determined as to whether
			// actual (true) or budget (false)
			draft_journal->set_whether_actual(is_actual);
			dj_actual_v_budget_determinations[draft_journal] =
				(is_actual? true: false);
		}
		else
		{
			// Bleugh!
			if (draft_journal->is_actual() != is_actual || (act_impact != decimal_zero && bud_impact != decimal_zero))
			{
				if (bud_impact == decimal_zero && act_impact == decimal_zero)
				{
					// Then it's OK - the journal just defaults to the
					// exising determination about whethet it's actual, as
					// it makes no difference
				}
				else
				{
					// We have a problem, and record this journal as
					// problematic
					problematic_draft_journals.insert(draft_journal);
					problematic_draft_journal_map[draft_journal] =
						draft_journal_name;
				}
			}
			else
			{
				assert
				(	dj_actual_v_budget_determinations[draft_journal] ==
					is_actual
				);
			}
		}
	}
	
	// Save the DraftJournal instances corresponding to draft journals into
	// the database.
	for
	(	DraftJournalVec::iterator jvit = draft_journal_vec.begin();
		jvit != draft_journal_vec.end();
		++jvit
	)
	{
		if (problematic_draft_journals.find(*jvit) != problematic_draft_journals.end())
		{
			// Any handling?
		}
		else
		{
			(*jvit)->save();
		}
	}

	// Read OrdinaryJournals*************
	
	// To hold specially constructed entry vectors into which
	// "mixed budget and actual" ordinary entries will be split
	// Use Nap draft journal name as key.
	unordered_map<int, vector<Entry> > special_actual_ordinary_vectors;
	unordered_map<int, vector<Entry> > special_budget_ordinary_vectors;


	std::ifstream ordinary_journal_csv
	(	(directory.string() + file_sep + ordinary_journal_csv_name).c_str()
	);
	string ordinary_journal_row;
	
	// We will store the OrdnaryJournal instances in this vector temporarily,
	// and run through later to ensure order is preserved. (The order they are
	// inserted into the database will thereby correspond with predicted id.)
	OrdinaryJournalVec ordinary_journal_vec;

	// We will store a map from the ordinary journal ids stored in the csv,
	// (which are the old ones that were assigned by N. A. P.) to
	// OrdinaryJournal instances in memory. Shortly this will enable us to
	// associate entries with OrdinaryJournal instances.
	unordered_map< int, shared_ptr<OrdinaryJournal> > ordinary_journal_map;

	// We will store a map from the old ordinary journal ids stored in the
	// csv, to Journal::Id values. This will enable us to remember the
	// PROSPECTIVE id of each OrdinaryJournal, so we can associate each
	// entry with the correct journal based on its N. A. P. journal id.
	unordered_map<int, Journal::Id> ordinary_journal_id_map;
	
	// Now to actually read the (non-draft, i.e. "ordinary") journals
	Journal::Id ordinary_journal_id = draft_journal_vec.size();
	vector<string> ordinary_journal_cells;
	while (getline(ordinary_journal_csv, ordinary_journal_row))
	{
		++ordinary_journal_id;

		// Split the csv row into cells
		easy_split<'|'>(ordinary_journal_row, ordinary_journal_cells);
		shared_ptr<OrdinaryJournal> ordinary_journal
		(	new OrdinaryJournal(*database_connection)
		);
		ordinary_journal->set_comment("");
		string const iso_date_string = ordinary_journal_cells[1];
		ordinary_journal->set_date
		(	boost::gregorian::date_from_iso_string(iso_date_string)
		);
		int const ordinary_journal_nap_id =
			lexical_cast<int>(ordinary_journal_cells[0].c_str());
		ordinary_journal_map[ordinary_journal_nap_id] = ordinary_journal;
		ordinary_journal_id_map[ordinary_journal_nap_id] =
			ordinary_journal_id;
		ordinary_journal_vec.push_back(ordinary_journal);

		special_actual_ordinary_vectors[ordinary_journal_nap_id]
			= vector<Entry>();
		special_budget_ordinary_vectors[ordinary_journal_nap_id]
			= vector<Entry>();
	}

	// Read ordinary entries ****************************************


	std::ifstream ordinary_entry_csv
	(	(directory.string() + file_sep + ordinary_entry_csv_name).c_str()
	);
	string ordinary_entry_row;
	vector<string> ordinary_entry_cells;
	while (getline(ordinary_entry_csv, ordinary_entry_row))
	{
		easy_split<'|'>(ordinary_entry_row, ordinary_entry_cells);
		Entry ordinary_entry(*database_connection);
		string const iso_date_string = ordinary_entry_cells[0];
		int const old_journal_id =
			lexical_cast<int>(ordinary_entry_cells[1].c_str());
		string const comment = ordinary_entry_cells[3];
		string const account_name = ordinary_entry_cells[4];
		Decimal const act_impact(ordinary_entry_cells[5]);
		Decimal const bud_impact(ordinary_entry_cells[6]);
		bool is_actual = (bud_impact == decimal_zero);
		ordinary_entry.set_account
		(	Account(*database_connection, account_name)
		);
		ordinary_entry.set_comment(comment);
		ordinary_entry.set_amount(is_actual? act_impact: -bud_impact);
		shared_ptr<OrdinaryJournal> ordinary_journal =
			ordinary_journal_map[old_journal_id];
		bool is_problematic = false;
		if
		(	oj_actual_v_budget_determinations.find(ordinary_journal) ==
			oj_actual_v_budget_determinations.end()
		)
		{
			// This journal has yet to be determined as to whether
			// actual (true) or budget (false)
			ordinary_journal->set_whether_actual(is_actual);
			oj_actual_v_budget_determinations[ordinary_journal] =
				(is_actual? true: false);
		}
		// Bleugh!
		if ((ordinary_journal->is_actual() != is_actual) || ((act_impact != decimal_zero) && (bud_impact != decimal_zero)))
		{
			if ((bud_impact == decimal_zero) && (act_impact == decimal_zero))
			{
				// Then it's OK
			}
			else
			{
				// We record the journal as problematic
				is_problematic = true;
				problematic_ordinary_journals.insert(ordinary_journal);
				problematic_ordinary_journal_map[ordinary_journal] =
					old_journal_id;
				
				// Split the problematic entry into an actual and a budget
				// part, as two distinct entries

				Entry special_actual_entry(*database_connection);
				special_actual_entry.set_account(ordinary_entry.account());
				special_actual_entry.set_comment(ordinary_entry.comment());
				special_actual_entry.set_amount(act_impact);
				special_actual_entry.set_whether_reconciled(false);
				
				Entry special_budget_entry(*database_connection);
				special_budget_entry.set_account(ordinary_entry.account());
				special_budget_entry.set_comment(ordinary_entry.comment());
				special_budget_entry.set_amount(-bud_impact);
				special_budget_entry.set_whether_reconciled(false);
				
				special_actual_ordinary_vectors[old_journal_id].push_back
				(	special_actual_entry
				);
				special_budget_ordinary_vectors[old_journal_id].push_back
				(	special_budget_entry
				);

				// Deal with any entries previously added to the journal
				if (!(ordinary_journal->entries().empty()))
				{
					JEWEL_DEBUG_LOG << "... " << ordinary_journal->entries().size() << endl;
					for
					(	vector<Entry>::const_iterator it = ordinary_journal->entries().begin();
						it != ordinary_journal->entries().end();
						++it
					)
					{	
						if (ordinary_journal->is_actual())
						{
							special_actual_ordinary_vectors[old_journal_id].push_back(*it);
						}
						else
						{
							special_budget_ordinary_vectors[old_journal_id].push_back(*it);
						}
					}
				}
							
			}
		}
		else
		{
			assert
			(	oj_actual_v_budget_determinations[ordinary_journal] ==
				is_actual
			);
		}

		if
		(	ordinary_journal_map[old_journal_id]->date() !=
			boost::gregorian::date_from_iso_string(iso_date_string)
		)
		{
			cout << "Error processing date of journal imported from nap."
			     << endl;
			std::abort();
		}

		if (!is_problematic)
		{
			ordinary_entry.set_whether_reconciled(false);
			ordinary_journal->add_entry(ordinary_entry);
		}
		else
		{
			JEWEL_DEBUG_LOG << "Entry recognised as problematic: " << endl;
			JEWEL_DEBUG_LOG << ordinary_entry.account().name() << "\t" << ordinary_entry.amount() << "\t" << endl;
		}
	}

	// Save the OrdinaryJournal instances corresponding to ordinary journals
	// into the database
	for
	(	OrdinaryJournalVec::iterator jvit = ordinary_journal_vec.begin();
		jvit != ordinary_journal_vec.end();
		++jvit
	)
	{
		if
		(	problematic_ordinary_journals.find(*jvit) !=
			problematic_ordinary_journals.end()
		)
		{
			// We have a problematic journal. We need to find all the
			// entries that belong with this journal. We need to create
			// two replacement journals to house these entries:
			// an actual one, and a draft one.
			
			// Process actual replacement journal
			OrdinaryJournal actual_replacement_oj(*database_connection);
			actual_replacement_oj.set_whether_actual(true);
			unordered_map<int, vector<Entry> >::iterator const act_it =
				special_actual_ordinary_vectors.find
				(	problematic_ordinary_journal_map[*jvit]
				);
			assert (act_it != special_actual_ordinary_vectors.end());
			cout << act_it->second.size() << endl;  // WARNING temp
			int test_ctr = 0; // WARNING temp
			for
			(	vector<Entry>::iterator e_it = act_it->second.begin();
				e_it != act_it->second.end();
				++e_it
			)
			{
				actual_replacement_oj.add_entry(*e_it);
				JEWEL_DEBUG_LOG << "Ctr: " << test_ctr++ << endl;
			}
			actual_replacement_oj.set_date((*jvit)->date());
			actual_replacement_oj.set_comment
			(	"Actual entries from former N. A. P. journal no. " +
				lexical_cast<string>(problematic_ordinary_journal_map[*jvit])
			);
			actual_replacement_oj.save();
			
			JEWEL_DEBUG_LOG << "Saved actual_replacement_oj: " << endl;
			for
			(	vector<Entry>::const_iterator it = actual_replacement_oj.entries().begin();
				it != actual_replacement_oj.entries().end();
				++it
			)
			{
				JEWEL_DEBUG_LOG << it->account().name() << "\t" << it->amount() << endl;
			}



			// Process budget replacement journal
			OrdinaryJournal budget_replacement_oj(*database_connection);
			budget_replacement_oj.set_whether_actual(false);
			unordered_map<int, vector<Entry> >::iterator const bud_it =
				special_budget_ordinary_vectors.find
				(	problematic_ordinary_journal_map[*jvit]
				);
			assert (bud_it != special_budget_ordinary_vectors.end());
			cout << bud_it->second.size() << endl;  // WARNING temp
			int test_ctrb = 0; // WARNING temp
			for
			(	vector<Entry>::iterator e_it = bud_it->second.begin();
				e_it != bud_it->second.end();
				++e_it
			)
			{
				budget_replacement_oj.add_entry(*e_it);
				JEWEL_DEBUG_LOG << "Ctr: " << test_ctrb++ << endl;
			}
			budget_replacement_oj.set_date((*jvit)->date());
			budget_replacement_oj.set_comment
			(	"Budget entries from former N. A. P. journal no. " +
				lexical_cast<string>(problematic_ordinary_journal_map[*jvit])
			);
			budget_replacement_oj.save();

			JEWEL_DEBUG_LOG << "Saved budget_replacement_oj: " << endl;
			for
			(	vector<Entry>::const_iterator it = budget_replacement_oj.entries().begin();
				it != budget_replacement_oj.entries().end();
				++it
			)
			{
				JEWEL_DEBUG_LOG << it->account().name() << "\t" << it->amount() << endl;
			}

		}
		else
		{
			(*jvit)->save();
		}
	}

	cout << "The following journals (by N. A. P. identifier) could not "
	     << "be imported automatically, and will need to be entered "
		 << "manually into Phatbooks:" << endl;
	/* The problematic _ordinary_ journals should have been captured now.
	for
	(	unordered_set< shared_ptr<OrdinaryJournal> >::const_iterator it =
			problematic_ordinary_journals.begin();
		it != problematic_ordinary_journals.end();
		++it
	)
	{
		cout << problematic_ordinary_journal_map[*it] << endl;
	}
	*/
	// There might still be problematic draft journals though...
	for
	(	unordered_set< shared_ptr<DraftJournal> >::const_iterator it =
		problematic_draft_journals.begin();
		it != problematic_draft_journals.end();
		++it
	)
	{
		cout << problematic_draft_journal_map[*it] << endl;
	}
	boost::posix_time::ptime const end_time =
		boost::posix_time::second_clock::local_time();
	cout << "Import ends at: " << end_time << endl;
	cout << "Import duration in seconds: "
	     << (end_time - start_time).total_seconds()
		 << endl;



	transaction.commit();
	return;
}
			

}  // namespace phatbooks





