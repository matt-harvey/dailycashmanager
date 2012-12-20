#include "ordinary_journal.hpp"
#include "ordinary_journal_impl.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

using boost::shared_ptr;
using sqloxx::Handle;
using std::string;
using std::vector;

// For debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;

namespace phatbooks
{

void
OrdinaryJournal::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	OrdinaryJournalImpl::setup_tables(dbc);
	return;
}

OrdinaryJournal::OrdinaryJournal
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_impl
	(	Handle<OrdinaryJournalImpl>(p_database_connection)
	)
{
}

OrdinaryJournal::OrdinaryJournal
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	m_impl
	(	Handle<OrdinaryJournalImpl>(p_database_connection, p_id)
	)
{
}

OrdinaryJournal
OrdinaryJournal::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return OrdinaryJournal
	(	Handle<OrdinaryJournalImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}


// TODO There is similar code in DraftJournal for this constructor.
// Factor out.
OrdinaryJournal::OrdinaryJournal
(	Journal& p_journal,
	PhatbooksDatabaseConnection& p_database_connection
):
	m_impl(Handle<OrdinaryJournalImpl>(p_database_connection))
{
	m_impl->set_whether_actual(p_journal.is_actual());
	m_impl->set_comment(p_journal.comment());
	for
	(	vector<Entry>::const_iterator it = p_journal.entries().begin();
		it != p_journal.entries().end();
		++it
	)
	{
		/*
		#ifdef DEBUG
			static int times_called = 0;
			                << "been called " << ++times_called << " times."
							<< endl;
		#endif
		*/
		Entry entry(*it);
		m_impl->add_entry(entry);
	}
}


void
OrdinaryJournal::set_whether_actual(bool p_is_actual)
{
	m_impl->set_whether_actual(p_is_actual);
	return;
}

void
OrdinaryJournal::set_comment(string const& p_comment)
{
	m_impl->set_comment(p_comment);
	return;
}

void
OrdinaryJournal::set_date(boost::gregorian::date const& p_date)
{
	m_impl->set_date(p_date);
	return;
}

void
OrdinaryJournal::add_entry(Entry& entry)
{
	m_impl->add_entry(entry);
	return;
}

bool
OrdinaryJournal::is_actual() const
{
	return m_impl->is_actual();
}

boost::gregorian::date
OrdinaryJournal::date() const
{
	return m_impl->date();
}

string
OrdinaryJournal::comment() const
{
	return m_impl->comment();
}

vector<Entry> const&
OrdinaryJournal::entries() const
{
	return m_impl->entries();
}

OrdinaryJournal::Id
OrdinaryJournal::id() const
{
	return m_impl->id();
}

void
OrdinaryJournal::save()
{
	m_impl->save();
	return;
}

// WARNING temp play
void
OrdinaryJournal::remove_first_entry()
{
	m_impl->remove_first_entry();
	return;
}


OrdinaryJournal::OrdinaryJournal
(	sqloxx::Handle<OrdinaryJournalImpl> const& p_handle
):
	m_impl(p_handle)
{
}


}  // namespace phatbooks
