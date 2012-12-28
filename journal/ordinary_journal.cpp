#include "ordinary_journal.hpp"
#include "draft_journal.hpp"
#include "draft_journal_impl.hpp"
#include "entry.hpp"
#include "ordinary_journal_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include <jewel/output_aux.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using boost::lexical_cast;
using boost::shared_ptr;
using jewel::output_aux;
using sqloxx::Handle;
using std::ios_base;
using std::ostream;
using std::ostringstream;
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


boost::gregorian::date
OrdinaryJournal::date() const
{
	return m_impl->date();
}



void
OrdinaryJournal::do_set_whether_actual(bool p_is_actual)
{
	m_impl->set_whether_actual(p_is_actual);
	return;
}

void
OrdinaryJournal::do_set_comment(string const& p_comment)
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
OrdinaryJournal::do_add_entry(Entry& entry)
{
	m_impl->add_entry(entry);
	return;
}

bool
OrdinaryJournal::do_get_whether_actual() const
{
	return m_impl->is_actual();
}

string
OrdinaryJournal::do_get_comment() const
{
	return m_impl->comment();
}

vector<Entry> const&
OrdinaryJournal::do_get_entries() const
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
OrdinaryJournal::mimic(ProtoJournal const& rhs)
{
	m_impl->mimic(rhs);
	return;
}

void
OrdinaryJournal::mimic(DraftJournal const& rhs)
{
	m_impl->mimic(rhs);
	return;
}


OrdinaryJournal::OrdinaryJournal
(	sqloxx::Handle<OrdinaryJournalImpl> const& p_handle
):
	m_impl(p_handle)
{
}


void
OrdinaryJournal::do_output(ostream& os) const
{
	os << date() << " ";
	// lexical_cast here avoids unwanted formatting
	os << "ORDINARY JOURNAL ID " << lexical_cast<string>(id()) << " ";
	Journal::do_output(os);
	return;
}





}  // namespace phatbooks
