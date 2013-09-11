// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "repeater.hpp"
#include "draft_journal.hpp"
#include "frequency.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "repeater_impl.hpp"
#include "repeater_table_iterator.hpp"
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/assert.hpp>
#include <wx/string.h>
#include <list>
#include <string>
#include <vector>

namespace gregorian = boost::gregorian;

using boost::lexical_cast;
using boost::shared_ptr;
using sqloxx::Handle;
using std::list;
using std::string;
using std::vector;

namespace phatbooks
{


void
Repeater::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	RepeaterImpl::setup_tables(dbc);
	return;
}

Repeater::Repeater
(	PhatbooksDatabaseConnection& p_database_connection
):
	PhatbooksPersistentObject(p_database_connection)
{
}

Repeater::Repeater
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
{
}


Repeater
Repeater::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return Repeater
	(	Handle<RepeaterImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}


void
Repeater::set_frequency(Frequency p_frequency)
{
	impl().set_frequency(p_frequency);
	return;
}

void
Repeater::set_journal_id(DraftJournal::Id p_journal_id)
{
	impl().set_journal_id(p_journal_id);
	return;
}

void
Repeater::set_next_date(gregorian::date const& p_next_date)
{
	impl().set_next_date(p_next_date);
	return;
}


Frequency
Repeater::frequency() const
{
	return impl().frequency();
}


gregorian::date
Repeater::next_date(vector<gregorian::date>::size_type n) const
{
	return impl().next_date(n);
}


shared_ptr<vector<gregorian::date> >
Repeater::firings_till(gregorian::date const& limit)
{
	return impl().firings_till(limit);
}

OrdinaryJournal
Repeater::fire_next()
{
	return impl().fire_next();
}

DraftJournal
Repeater::draft_journal() const
{
	return impl().draft_journal();
}


Repeater::Repeater(sqloxx::Handle<RepeaterImpl> const& p_handle):
	PhatbooksPersistentObject(p_handle)
{
}

void
Repeater::mimic(Repeater const& rhs)
{
	impl().mimic(rhs.impl());
	return;
}





// Implement free functions

namespace
{
	bool
	is_earlier_than(OrdinaryJournal const& lhs, OrdinaryJournal const& rhs)
	{
		return lhs.date() < rhs.date();
	}
}  // End anonymous namespace


shared_ptr<list<OrdinaryJournal> >
update_repeaters(PhatbooksDatabaseConnection& dbc, gregorian::date d)
{
	shared_ptr<list<OrdinaryJournal> > auto_posted_journals
	(	new list<OrdinaryJournal>
	);
	// Read into a vector first - uneasy about reading and writing
	// at the same time.
	RepeaterTableIterator rtit(dbc);
	RepeaterTableIterator const rtend;
	vector<Repeater> vec(rtit, rtend);

	// Bare scope
	{
		vector<Repeater>::iterator it = vec.begin();
		vector<Repeater>::iterator const end = vec.end();
		for ( ; it != end; ++it)
		{
			while (it->next_date() <= d)
			{
				OrdinaryJournal const oj = it->fire_next();
				// In the special case where oj is dbc.budget_instrument(),
				// and is
				// devoid of entries, firing it does not cause any
				// OrdinaryJournal to be posted, but simply advances
				// the next posting date. In this case the returned
				// OrdinaryJournal will have no id.
#				ifndef NDEBUG
					DraftJournal const dj = it->draft_journal();
					DraftJournal const bi = dbc.budget_instrument();
#				endif
				if (oj.has_id())
				{
					JEWEL_ASSERT (dj != bi || !dj.entries().empty());
					auto_posted_journals->push_back(oj);
				}
				else
				{
					JEWEL_ASSERT (dj == bi);
					JEWEL_ASSERT (dj.entries().empty());
					JEWEL_ASSERT (oj.entries().empty());
				}
			}
		}
	}
	auto_posted_journals->sort(is_earlier_than);
	return auto_posted_journals;
}




}  // namespace phatbooks
