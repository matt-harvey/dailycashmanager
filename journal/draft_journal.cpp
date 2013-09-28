// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "draft_journal.hpp"
#include "draft_journal_impl.hpp"
#include "draft_journal_table_iterator.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "transaction_type.hpp"
#include <sqloxx/handle.hpp>
#include <boost/lexical_cast.hpp>
#include <wx/string.h>
#include <iostream>
#include <ostream>
#include <string>

using boost::lexical_cast;
using std::endl;
using std::ostream;
using std::string;
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
	wxString const& p_name
)
{
	return DraftJournalImpl::exists
	(	p_database_connection,
		p_name
	);
}

bool
DraftJournal::no_user_draft_journals_saved
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	DraftJournalTableIterator const it =
		make_name_ordered_user_draft_journal_table_iterator
		(	p_database_connection
		);
	DraftJournalTableIterator const end;
	return it == end;
}

void
DraftJournal::set_name(wxString const& p_name)
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
DraftJournal::do_set_transaction_type
(	transaction_type::TransactionType p_transaction_type
)
{
	impl().set_transaction_type(p_transaction_type);
	return;
}

void
DraftJournal::do_set_comment(wxString const& p_comment)
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

transaction_type::TransactionType
DraftJournal::do_get_transaction_type() const
{
	return impl().transaction_type();
}

wxString
DraftJournal::do_get_comment() const
{
	return impl().comment();
}

wxString
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

wxString
DraftJournal::repeater_description() const
{
	return impl().repeater_description();
}

DraftJournal::DraftJournal
(	Handle<DraftJournalImpl> const& p_handle
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
	os << "DRAFT JOURNAL ";
	if (has_id())
	{
		os << "ID " << lexical_cast<string>(id()) << " ";
	}
	os << "NAME " << name() << " ";
	PersistentJournal::do_output(os);
	os << endl << repeater_description() << endl;
	return;
}



}  // namespace phatbooks
