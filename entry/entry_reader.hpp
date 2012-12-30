#ifndef GUARD_entry_reader_hpp
#define GUARD_entry_reader_hpp

#include "date.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <sqloxx/reader.hpp>
#include <sqloxx/sql_statement.hpp>

namespace phatbooks
{

class Account;


typedef
	sqloxx::Reader<Entry, PhatbooksDatabaseConnection>
	EntryReader;


class OrdinaryEntryReader: public EntryReader
{
public:

	explicit OrdinaryEntryReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	static boost::shared_ptr<OrdinaryEntryReader>
	provide_date_filtered_reader_for_account
	(	PhatbooksDatabaseConnection& p_database_connection,
		Account const& account,
		boost::optional<boost::gregorian::date> earliest =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> latest =
			boost::optional<boost::gregorian::date>()
	);

	static boost::shared_ptr<OrdinaryEntryReader>
	provide_date_filtered_reader
	(	PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> earliest =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> latest =
			boost::optional<boost::gregorian::date>()
	);

private:
	OrdinaryEntryReader
	(	PhatbooksDatabaseConnection& p_database_connection,
		sqloxx::SQLStatement& p_statement
	);
};




}  // namespace phatbooks


#endif  // GUARD_entry_reader_hpp
