#include "ordinary_journal.hpp"
#include "draft_journal.hpp"
#include "draft_journal_impl.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "ordinary_journal_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

using boost::lexical_cast;
using boost::shared_ptr;
using sqloxx::Handle;
using std::ios_base;
using std::ostream;
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

bool
OrdinaryJournal::is_balanced() const
{
	return m_impl->is_balanced();
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


void
OrdinaryJournal::mimic(Journal const& rhs)
{
	// TODO Now this is written, I can do away
	// with the constructor that takes a Journal const&.
	m_impl->mimic(rhs);
	return;
}

void
OrdinaryJournal::mimic(DraftJournal const& rhs)
{
	m_impl->mimic(rhs);
	return;
}

void
OrdinaryJournal::mimic(OrdinaryJournal const& rhs)
{
	m_impl->mimic(*(rhs.m_impl));
	return;
}

void
OrdinaryJournal::clear_entries()
{
	m_impl->clear_entries();
	return;
}

OrdinaryJournal::OrdinaryJournal
(	sqloxx::Handle<OrdinaryJournalImpl> const& p_handle
):
	m_impl(p_handle)
{
}

ostream&
OrdinaryJournal::do_output(ostream& os) const
{
	os << date();
	// lexical_cast here avoids unwanted formatting
	os << " JOURNAL ID " << lexical_cast<string>(id()) << " ";
	return Journal::do_output(os);
}



}  // namespace phatbooks
