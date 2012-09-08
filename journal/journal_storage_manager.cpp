#include "journal_storage_manager.hpp"
#include "account.hpp"
#include "account_storage_manager.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sql_statement.hpp"
#include <boost/cstdint.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <string>

using phatbooks::Account;
using phatbooks::Commodity;
using phatbooks::DateType;
using phatbooks::Entry;
using phatbooks::Journal;
using phatbooks::IdType;
using jewel::Decimal;
using boost::numeric_cast;
using boost::shared_ptr;
using std::list;
using std::string;

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
	(	Iter it = journal.entries().begin();
		it != journal.entries().end();
		++it
	)
	{
		SQLStatement account_id_finder
		(	dbc,
			"select account_id from accounts where name = :aname"
		);
		account_id_finder.bind(":aname", (*it)->account_name());
		account_id_finder.step();
		SQLStatement entry_storer
		(	dbc,	
			"insert into entries(journal_id, comment, account_id, "
			"amount) values(:journal_id, :comment, :account_id, :amount)"
		);
		entry_storer.bind(":journal_id", journal_id);
		entry_storer.bind(":comment", (*it)->comment());
		entry_storer.bind
		(	":account_id",
			account_id_finder.extract<IdType>(0)
		);
		entry_storer.bind(":amount", (*it)->amount().intval());
		entry_storer.quick_step();
	}
	dbc.execute_sql("end transaction;");
	return;
}


phatbooks::Journal
StorageManager<Journal>::load(Key const& id, DatabaseConnection& dbc)
{
	SQLStatement statement
	(	dbc,
		"select journal_id, is_actual, date, comment from "
		"journals where journal_id = :key"
	);
	statement.bind(":key", id);
	statement.step();
	Journal ret;
	ret.set_whether_actual
	(	static_cast<bool>(statement.extract<int>(1))
	);
	ret.set_date
	(	numeric_cast<DateType>
		(	statement.extract<boost::int64_t>(2)
		)
	);
	ret.set_comment(statement.extract<string>(3));
	
	SQLStatement entry_finder
	(	dbc,
		"select entry_id, comment, account_id, amount from entries where "
		"journal_id = :jid"
	);
	entry_finder.bind(":jid", id);
	while (entry_finder.step())
	{	
		SQLStatement account_finder
		(	dbc,
			"select name, precision from "
			"accounts_extended where account_id = :aid"
		);
		account_finder.bind(":aid", entry_finder.extract<IdType>(2));
		shared_ptr<Entry> entry
		(	new Entry
			(	account_finder.extract<string>(0),
				entry_finder.extract<string>(2),
				Decimal
				(	entry_finder.extract<boost::int64_t>(3),
					account_finder.extract<int>(1)
				)
			)
		);
		ret.add_entry(entry);
	}
	return ret;
}

void
StorageManager<Journal>::setup_tables(DatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table journals"
		"("
			"journal_id integer primary key autoincrement, "
			"is_actual integer not null references booleans, "
			"date integer not null, "
			"comment text"
		");"
	);
	dbc.execute_sql
	(	"create table entries"
		"("
			"entry_id integer primary key autoincrement, "
			"journal_id not null references journals, "
			"comment text, "
			"account_id not null references accounts, "
			"amount integer not null "
		");"
	);
	return;
}

}  // namespace sqloxx
