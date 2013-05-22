// Copyright (c) 2013, Matthew Harvey. All rights reserved.

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
	assert (s_num_instances <= s_max_instances);
}


Session::~Session()
{
	assert (s_num_instances > 0);
	assert (s_num_instances <= s_max_instances);
	--s_num_instances;
}


int
Session::run()
{
	return do_run();
}


int
Session::run(string const& filepath_str)
{
	if (filepath_str.empty())
	{
		throw InvalidFilename("Filename is empty string.");
	}	
	return do_run(filepath_str);
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
	// Anonymous scope
	{
		RepeaterReader::const_iterator const end = repeater_reader.end();
		for
		(	RepeaterReader::iterator it = repeater_reader.begin();
			it != end;
			++it
		)
		{
			while (it->next_date() <= d)
			{
				OrdinaryJournal const oj = it->fire_next();
				// In the special case where oj is
				// database_connection().budget_instrument(), and is
				// devoid of entries, firing it does not cause any
				// OrdinaryJournal to be posted, but simply advances
				// the next posting date. In this case the returned
				// OrdinaryJournal will have no id.
#				ifndef NDEBUG
					DraftJournal const dj = it->draft_journal();
					DraftJournal const bi =
						database_connection().budget_instrument();
#				endif
				if (oj.has_id())
				{
					assert(dj != bi || !dj.entries().empty());
					auto_posted_journals->push_back(oj);
				}
				else
				{
					assert (dj == bi);
					assert (dj.entries().empty());
					assert (oj.entries().empty());
				}
			}
		}
	}
	auto_posted_journals->sort(is_earlier_than);

	return auto_posted_journals;
}

			
	





}  // namespace phatbooks
