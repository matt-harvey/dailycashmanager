#include "journal.hpp"
#include "repeater.hpp"
#include "repeater_impl.hpp"
#include "repeater_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/handle.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;
using sqloxx::Handle;

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

Repeater::Repeater(RepeaterReader const& p_reader):
	m_impl(p_reader.handle())
{
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
Repeater::set_journal_id(Journal::Id p_journal_id)
{
	m_impl->set_journal_id(p_journal_id);
	return;
}

void
Repeater::set_next_date(boost::gregorian::date const& p_next_date)
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

boost::gregorian::date
Repeater::next_date() const
{
	return m_impl->next_date();
}

Journal::Id
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

}  // namespace phatbooks
