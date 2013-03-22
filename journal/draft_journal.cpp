#include "b_string.hpp"
#include "draft_journal.hpp"
#include "draft_journal_impl.hpp"
#include "draft_journal_reader.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include <sqloxx/handle.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <ostream>

using boost::shared_ptr;
using std::endl;
using std::ostream;
using std::vector;
using sqloxx::Handle;

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
	PhatbooksPersistentObject(p_database_connection)
{
}

DraftJournal::DraftJournal
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
{
}


DraftJournal
DraftJournal::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return DraftJournal
	(	Handle<DraftJournalImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}

bool
DraftJournal::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	BString const& p_name
)
{
	return DraftJournalImpl::exists
	(	p_database_connection,
		p_name
	);
}

void
DraftJournal::set_name(BString const& p_name)
{
	impl().set_name(p_name);
	return;
}


void
DraftJournal::push_repeater(Repeater& repeater)
{
	impl().push_repeater(repeater);
	return;
}

bool
DraftJournal::has_repeaters() const
{
	return impl().has_repeaters();
}

void
DraftJournal::clear_repeaters()
{
	impl().clear_repeaters();
	return;
}

void
DraftJournal::do_clear_entries()
{
	impl().clear_entries();
	return;
}

void
DraftJournal::do_set_whether_actual(bool p_is_actual)
{
	impl().set_whether_actual(p_is_actual);
	return;
}

void
DraftJournal::do_set_comment(BString const& p_comment)
{
	impl().set_comment(p_comment);
	return;
}

void
DraftJournal::do_push_entry(Entry& entry)
{
	impl().push_entry(entry);
	return;
}

void
DraftJournal::do_remove_entry(Entry& entry)
{
	impl().remove_entry(entry);
	return;
}

bool
DraftJournal::do_get_whether_actual() const
{
	JEWEL_DEBUG_LOG << __FILE__ << __LINE__ << endl;
	return impl().is_actual();
}

BString
DraftJournal::do_get_comment() const
{
	return impl().comment();
}
	
BString
DraftJournal::name() const
{
	return impl().name();
}

vector<Entry> const&
DraftJournal::do_get_entries() const
{
	return impl().entries();
}



vector<Repeater> const&
DraftJournal::repeaters() const
{
	return impl().repeaters();
}


BString
DraftJournal::repeater_description() const
{
	return impl().repeater_description();
}

DraftJournal::DraftJournal
(	sqloxx::Handle<DraftJournalImpl> const& p_handle
):
	PhatbooksPersistentObject(p_handle)
{
}

void
DraftJournal::mimic(ProtoJournal const& rhs)
{
	impl().mimic(rhs);
	return;
}

void
DraftJournal::mimic(DraftJournal const& rhs)
{
	impl().mimic(rhs.impl());
	return;
}

void
DraftJournal::do_output(ostream& os) const
{
	os << "DRAFT JOURNAL ID " << id() << " "
	   << " NAME " << name() << " ";
	PersistentJournal::do_output(os);
	os << endl << repeater_description() << endl;
	return;
}



}  // namespace phatbooks
