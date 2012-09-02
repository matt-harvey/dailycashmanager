#include "journal_storage_manager.hpp"
#include "database_connection.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <list>

using phatbooks::Entry;
using phatbooks::Journal;
using phatbooks::IdType;
using boost::shared_ptr;
using std::list;

namespace sqloxx
{


void StorageManager<Journal>::save
(	Journal const& journal,
	DatabaseConnection& dbc
)
{
	IdType const journal_id = dbc.next_auto_key<IdType>("journals");
	dbc.execute_sql("begin transaction;");
	SQLStatement statement
	(	dbc,
		"insert into journals(is_actual, date, comment) "
		"values(:is_actual, :date, :comment)"
	);
	statement.bind(":is_actual", static_cast<int>(journal.is_actual()));
	statement.bind(":date", journal.date());
	statement.bind(":comment", journal.comment());
	statement.quick_step();

	// WARNING I should make this more efficient by setting up the 
	// account_id_finder and entry_storer outside the loop, and then
	// just resetting within the loop. Better, there could be a view with
	// some of this this stuff already worked out in the view, and I
	// could just query that view.
	typedef list< shared_ptr<Entry> > EntryCntnr;
	typedef EntryCntnr::const_iterator Iter;
	for
	(	Iter it = journal.m_entries.begin();
		it != journal.m_entries.end();
		++it
	)
	{
		SQLStatement account_id_finder
		(	dbc,
			"select account_id from accounts where name = :aname"
		);
		account_id_finder.bind(":aname", (*it)->account_name());
		account_id_finder.step();
		IdType acct_id = account_id_finder.extract<IdType>(0);
		SQLStatement entry_storer
		(	dbc,	
			"insert into entries(journal_id, comment, account_id, "
			"amount) values(:journal_id, :comment, :account_id, :amount)"
		);
		entry_storer.bind(":journal_id", journal_id);
		entry_storer.bind(":comment", (*it)->comment());
		entry_storer.bind(":account_id", acct_id);
		entry_storer.bind(":amount", (*it)->amount().intval());
		entry_storer.quick_step();
	}
	dbc.execute_sql("end transaction;");
	return;
}


}  // namespace sqloxx
