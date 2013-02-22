#include "phatbooks_database_connection.hpp"
#include "entry_reader.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_exceptions.hpp"
#include "repeater.hpp"
#include "repeater_reader.hpp"
#include "session.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <cassert>
#include <list>
#include <string>

#ifdef DEBUG
#	include <jewel/debug_log.hpp>
#	include <iostream>
#endif

using boost::shared_ptr;
using sqloxx::InvalidFilename;
using std::list;
using std::string;
namespace gregorian = boost::gregorian;

namespace phatbooks
{

int Session::s_num_instances = 0;

Session::Session():
	m_database_connection(new PhatbooksDatabaseConnection)
{
	database_connection().set_caching_level(s_default_caching_level);
	++s_num_instances;
	if (s_num_instances > s_max_instances)
	{
		--s_num_instances;
		throw TooManySessions
		(	"Exceeded maximum number of instances of phatbooks::Session."
		);
	}
	assert (s_num_instances >= s_max_instances);
}


Session::~Session()
{
	assert (s_num_instances > 0);
	assert (s_num_instances <= s_max_instances);
	--s_num_instances;
}


int
Session::run(string const& p_filename)
{
	if (p_filename.empty())
	{
		throw InvalidFilename("Filename is empty string.");
	}	
	return do_run(p_filename);
}


PhatbooksDatabaseConnection&
Session::database_connection() const
{
	return *m_database_connection;
}


namespace
{
	bool
	is_earlier_than(OrdinaryJournal const& lhs, OrdinaryJournal const& rhs)
	{
		return lhs.date() < rhs.date();
	}
}  // End anonymous namespace


shared_ptr<list<OrdinaryJournal> >
Session::update_repeaters_till(gregorian::date d)
{
	shared_ptr<list<OrdinaryJournal> > auto_posted_journals
	(	new list<OrdinaryJournal>
	);

	RepeaterReader repeater_reader(database_connection());
	for
	(	RepeaterReader::iterator it = repeater_reader.begin(),
			end = repeater_reader.end();
		it != end;
		++it
	)
	{
		while (it->next_date() <= d)
		{
			auto_posted_journals->push_back(it->fire_next());
		}
	}
	auto_posted_journals->sort(is_earlier_than);

	return auto_posted_journals;
}

			
	





}  // namespace phatbooks
