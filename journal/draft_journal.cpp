#include "draft_journal.hpp"
#include "draft_journal_impl.hpp"
#include "draft_journal_reader.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/handle.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using boost::shared_ptr;
using std::string;
using std::vector;
using sqloxx::get_handle;

namespace phatbooks
{

void
DraftJournal::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	DraftJournalImpl::setup_tables(dbc);
	return;
}

DraftJournal::DraftJournal
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_impl(get_handle<DraftJournalImpl>(p_database_connection))
{
}

DraftJournal::DraftJournal
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	m_impl(get_handle<DraftJournalImpl>(p_database_connection, p_id))
{
}


// TODO There is similar code in OrdinaryJournal for this constructor.
// Factor out.
DraftJournal::DraftJournal
(	Journal& p_journal,
	PhatbooksDatabaseConnection& p_database_connection
):
	m_impl(get_handle<DraftJournalImpl>(p_database_connection))
{
	m_impl->set_whether_actual(p_journal.is_actual());
	m_impl->set_comment(p_journal.comment());
	for
	(	vector<Entry>::const_iterator it = p_journal.entries().begin();
		it != p_journal.entries().end();
		++it
	)
	{
		Entry entry(*it);
		m_impl->add_entry(entry);
	}
}

DraftJournal::DraftJournal(DraftJournalReader const& p_reader):
	m_impl(p_reader.handle())
{
}

void
DraftJournal::set_whether_actual(bool p_is_actual)
{
	m_impl->set_whether_actual(p_is_actual);
	return;
}

void
DraftJournal::set_comment(string const& p_comment)
{
	m_impl->set_comment(p_comment);
	return;
}

void
DraftJournal::set_name(string const& p_name)
{
	m_impl->set_name(p_name);
	return;
}

void
DraftJournal::add_entry(Entry& entry)
{
	m_impl->add_entry(entry);
	return;
}

void
DraftJournal::add_repeater(Repeater& repeater)
{
	m_impl->add_repeater(repeater);
	return;
}

bool
DraftJournal::is_actual() const
{
	return m_impl->is_actual();
}

string
DraftJournal::comment() const
{
	return m_impl->comment();
}

string
DraftJournal::name() const
{
	return m_impl->name();
}

vector<Entry> const&
DraftJournal::entries() const
{
	return m_impl->entries();
}

void
DraftJournal::save()
{
	m_impl->save();
	return;
}

DraftJournal::Id
DraftJournal::id() const
{
	return m_impl->id();
}



}  // namespace phatbooks
