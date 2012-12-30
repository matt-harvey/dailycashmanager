#include "account.hpp"
#include "date.hpp"
#include "entry_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/reader.hpp>
#include <string>

using boost::gregorian::date;
using boost::optional;
using jewel::value;
using std::string;

namespace phatbooks
{



namespace
{
	string const text_core =
		"select entry_id from entries inner join ordinary_journal_detail "
		"using(journal_id) ";

	string const account_filtering_text = "account_id = :account_id ";

	string const date_filtering_text =
		"(date >= earliest: and date <= :latest) ";
	
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

}  // End anonymous namespace


OrdinaryEntryReader::OrdinaryEntryReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	EntryReader(p_database_connection, ordinary_reader_text)
{
}
	


FilteredOrdinaryEntryReader::FilteredOrdinaryEntryReader
(	PhatbooksDatabaseConnection& p_database_connection,
	Account const& account,
	optional<date> earliest,
	optional<date> latest
):
	EntryReader(p_database_connection, filtered_reader_text_with_account)
{
	bind(":account_id", account.id());
	bind_dates(earliest, latest);
}

FilteredOrdinaryEntryReader::FilteredOrdinaryEntryReader
(	PhatbooksDatabaseConnection& p_database_connection,
	optional<date> earliest,
	optional<date> latest
):
	EntryReader(p_database_connection, filtered_reader_text_without_account)
{
	bind_dates(earliest, latest);
}
	

void
FilteredOrdinaryEntryReader::bind_dates
(	optional<date> earliest,
	optional<date> latest
)
{
	if (earliest) bind(":earliest", julian_int(value(earliest)));
	else bind(":earliest", earliest_date_rep());

	if (latest) bind(":latest", julian_int(value(latest)));
	else bind(":latest", latest_date_rep());
}





}  // namespace phatbooks
