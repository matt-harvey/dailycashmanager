#include "repeater.hpp"
#include "frequency.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "repeater_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "b_string.hpp"
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

DraftJournal::Id
Repeater::journal_id() const
{
	return impl().journal_id();
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

}  // namespace phatbooks
