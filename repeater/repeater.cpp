#include "ordinary_journal.hpp"
#include "proto_journal.hpp"
#include "repeater.hpp"
#include "repeater_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

namespace gregorian = boost::gregorian;

using boost::lexical_cast;
using boost::shared_ptr;
using sqloxx::Handle;
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
	m_impl
	(	Handle<RepeaterImpl>(p_database_connection)
	)
{
}

Repeater::Repeater
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	m_impl
	(	Handle<RepeaterImpl>(p_database_connection, p_id)
	)
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
Repeater::set_interval_type(IntervalType p_interval_type)
{
	m_impl->set_interval_type(p_interval_type);
	return;
}

void
Repeater::set_interval_units(int p_interval_units)
{
	m_impl->set_interval_units(p_interval_units);
	return;
}

void
Repeater::set_journal_id(ProtoJournal::Id p_journal_id)
{
	m_impl->set_journal_id(p_journal_id);
	return;
}

void
Repeater::set_next_date(gregorian::date const& p_next_date)
{
	m_impl->set_next_date(p_next_date);
	return;
}


Repeater::IntervalType
Repeater::interval_type() const
{
	return m_impl->interval_type();
}

int
Repeater::interval_units() const
{
	return m_impl->interval_units();
}

gregorian::date
Repeater::next_date(vector<gregorian::date>::size_type n) const
{
	return m_impl->next_date(n);
}


shared_ptr<vector<gregorian::date> >
Repeater::firings_till(gregorian::date const& limit)
{
	return m_impl->firings_till(limit);
}

OrdinaryJournal
Repeater::fire_next()
{
	return m_impl->fire_next();
}

ProtoJournal::Id
Repeater::journal_id() const
{
	return m_impl->journal_id();
}

Repeater::Id
Repeater::id() const
{
	return m_impl->id();
}

void
Repeater::save()
{
	m_impl->save();
	return;
}

void
Repeater::remove()
{
	m_impl->remove();
	return;
}

void
Repeater::ghostify()
{
	m_impl->ghostify();
	return;
}

Repeater::Repeater(sqloxx::Handle<RepeaterImpl> const& p_handle):
	m_impl(p_handle)
{
}


string
frequency_description(Repeater const& repeater)
{
	string ret = "every ";
	int const units = repeater.interval_units();	
	if (units > 1)
	{
		ret += lexical_cast<string>(units);
		ret += " ";
		ret += phrase(repeater.interval_type(), true);
	}
	else
	{
		ret += phrase(repeater.interval_type(), false);
	}
	return ret;
}



}  // namespace phatbooks
