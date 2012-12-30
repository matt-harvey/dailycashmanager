#include "account.hpp"
#include "date.hpp"
#include "entry_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/reader.hpp>
#include <sqloxx/sql_statement.hpp>
#include <string>

using boost::gregorian::date;
using boost::optional;
using boost::shared_ptr;
using jewel::value;
using sqloxx::SQLStatement;
using std::string;

namespace phatbooks
{



namespace
{
	string const param_name_account_id = ":account_id";
	string const param_name_earliest = ":earliest";
	string const param_name_latest = ":latest";

	string const text_core =
		"select entry_id from entries inner join ordinary_journal_detail "
		"using(journal_id) ";

	string const account_filtering_text =
		"account_id = " +
		param_name_account_id +
		" ";

	string const date_filtering_text =
		"(date >= " +
		param_name_earliest +
		" and date <= " +
		param_name_latest +
		") ";
	
	string const ordering_text = "order by date ";

	string const ordinary_reader_text = text_core;

	string const filtered_reader_text_with_account =
		text_core +
		"where " +
		account_filtering_text +
		"and " +
		date_filtering_text +
		ordering_text;
	
	string const filtered_reader_text_without_account =
		text_core +
		"where " +
		date_filtering_text +
		ordering_text;


	void bind_dates
	(	SQLStatement& statement,
		optional<date> earliest,
		optional<date> latest
	)
	{
		if (earliest)
		{
			statement.bind(param_name_earliest, julian_int(value(earliest)));
		}
		else
		{
			statement.bind(param_name_earliest, earliest_date_rep());
		}

		if (latest)
		{
			statement.bind(param_name_latest, julian_int(value(latest)));
		}
		else
		{
			statement.bind(param_name_latest, latest_date_rep());
		}
		return;
	}


}  // End anonymous namespace


OrdinaryEntryReader::OrdinaryEntryReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	EntryReader(p_database_connection, ordinary_reader_text)
{
}

OrdinaryEntryReader::OrdinaryEntryReader
(	PhatbooksDatabaseConnection& p_database_connection,
	SQLStatement& p_statement
):
	EntryReader(p_database_connection, p_statement)
{
}


shared_ptr<OrdinaryEntryReader>
OrdinaryEntryReader::provide_date_filtered_reader_for_account
(	PhatbooksDatabaseConnection& p_database_connection,
	Account const& account,
	optional<date> earliest,
	optional<date> latest
)
{
	SQLStatement statement
	(	p_database_connection,
		filtered_reader_text_with_account
	);
	statement.bind(param_name_account_id, account.id());
	bind_dates(statement, earliest, latest);
	shared_ptr<OrdinaryEntryReader> ret
	(	new OrdinaryEntryReader(p_database_connection, statement)
	);
	return ret;	
}

shared_ptr<OrdinaryEntryReader>
OrdinaryEntryReader::provide_date_filtered_reader
(	PhatbooksDatabaseConnection& p_database_connection,
	optional<date> earliest,
	optional<date> latest
)
{
	SQLStatement statement
	(	p_database_connection,
		filtered_reader_text_without_account
	);
	bind_dates(statement, earliest, latest);
	shared_ptr<OrdinaryEntryReader> ret
	(	new OrdinaryEntryReader(p_database_connection, statement)
	);
	return ret;
}


}  // namespace phatbooks
