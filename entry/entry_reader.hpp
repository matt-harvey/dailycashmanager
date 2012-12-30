#ifndef GUARD_entry_reader_hpp
#define GUARD_entry_reader_hpp

#include "date.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <sqloxx/reader.hpp>

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
};


class FilteredOrdinaryEntryReader: public EntryReader
{
public:	
	FilteredOrdinaryEntryReader
	(	PhatbooksDatabaseConnection& p_database_connection,
		Account const& account,
		boost::optional<boost::gregorian::date> earliest =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> latest =
			boost::optional<boost::gregorian::date>()
	);
	explicit FilteredOrdinaryEntryReader
	(	PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> earliest =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> latest =
			boost::optional<boost::gregorian::date>()
	);
private:
	void bind_dates
	(	boost::optional<boost::gregorian::date> earliest,
		boost::optional<boost::gregorian::date> latest
	);

};



}  // namespace phatbooks


#endif  // GUARD_entry_reader_hpp
