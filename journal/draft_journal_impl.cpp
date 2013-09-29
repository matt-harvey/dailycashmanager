// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "string_conv.hpp"
#include "draft_journal.hpp"
#include "draft_journal_impl.hpp"
#include "entry_handle.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "proto_journal.hpp"
#include "repeater.hpp"
#include "string_conv.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/string.h>
#include <string>
#include <unordered_set>
#include <vector>

namespace gregorian = boost::gregorian;

using boost::lexical_cast;
using boost::optional;
using jewel::clear;
using jewel::value;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::unordered_set;
using std::vector;
using std::string;

namespace phatbooks
{

string
DraftJournalImpl::primary_table_name()
{
	return ProtoJournal::primary_table_name();
}

string
DraftJournalImpl::exclusive_table_name()
{
	return "draft_journal_detail";
}

string
DraftJournalImpl::primary_key_name()
{
	return ProtoJournal::primary_key_name();
}

void
DraftJournalImpl::set_transaction_type
(	TransactionType p_transaction_type
)
{
	load();
	ProtoJournal::set_transaction_type(p_transaction_type);
	return;
}

void
DraftJournalImpl::set_comment(wxString const& p_comment)
{
	load();
	ProtoJournal::set_comment(p_comment);
	return;
}

void
DraftJournalImpl::push_entry(EntryHandle const& entry)
{
	load();
	if (has_id())
	{
		entry->set_journal_id(id());
	}
	ProtoJournal::push_entry(entry);
	return;
}

void
DraftJournalImpl::remove_entry(EntryHandle const& entry)
{
	load();
	ProtoJournal::remove_entry(entry);
	return;
}

TransactionType
DraftJournalImpl::transaction_type()
{
	load();
	return ProtoJournal::transaction_type();
}

wxString
DraftJournalImpl::comment()
{
	load();
	return ProtoJournal::comment();
}

vector<EntryHandle> const&
DraftJournalImpl::entries()
{
	load();
	// WARNING Should this fail if m_entries is empty? This would
	// be the same behaviour then as the other "optionals". To be
	// truly consistent with the other optionals, it would fail
	// by means of a failed assert (assuming I haven't wrapped the
	// other optionals in some throwing construct...).
	return ProtoJournal::entries();
}


vector<Repeater> const&
DraftJournalImpl::repeaters()
{
	load();
	return m_dj_data->repeaters;
}

void
DraftJournalImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table draft_journal_detail"
		"("
			"journal_id integer primary key references journals, "
			"name text not null unique"
		")"
	);
	return;
}


DraftJournalImpl::DraftJournalImpl
(	IdentityMap& p_identity_map
):
	DraftJournalImpl::PersistentObject(p_identity_map),
	ProtoJournal(),
	m_dj_data(new DraftJournalData)
{
}


DraftJournalImpl::DraftJournalImpl
(	IdentityMap& p_identity_map,	
	Id p_id
):
	DraftJournalImpl::PersistentObject(p_identity_map, p_id),
	ProtoJournal(),
	m_dj_data(new DraftJournalData)
{
}

bool
DraftJournalImpl::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_name
)
{
	wxString const target = p_name.Lower();
	SQLStatement statement
	(	p_database_connection,
		"select name from draft_journal_detail"
	);
	while (statement.step())
	{
		wxString const candidate =
			std8_to_wx(statement.extract<string>(0)).Lower();
		if (candidate == target)
		{
			return true;
		}
	}
	return false;
}

void
DraftJournalImpl::set_name(wxString const& p_name)
{
	load();
	m_dj_data->name = p_name;
	return;
}


void
DraftJournalImpl::push_repeater(Repeater& repeater)
{
	load();
	if (has_id())
	{
		repeater.set_journal_id(id());
	}
	m_dj_data->repeaters.push_back(repeater);
	return;
}


wxString
DraftJournalImpl::name()
{
	load();
	return value(m_dj_data->name);
}


DraftJournalImpl::DraftJournalImpl(DraftJournalImpl const& rhs):
	DraftJournalImpl::PersistentObject(rhs),
	ProtoJournal(rhs),
	m_dj_data(new DraftJournalData(*(rhs.m_dj_data)))
{
}


void
DraftJournalImpl::swap(DraftJournalImpl& rhs)
{
	swap_base_internals(rhs);
	ProtoJournal::swap(rhs);
	using std::swap;
	swap(m_dj_data, rhs.m_dj_data);
	return;
}


void
DraftJournalImpl::do_load()
{
	DraftJournalImpl temp(*this);
	
	// Load the base part of temp.
	temp.do_load_journal_core(database_connection(), id());

	// Load the derived, DraftJournalImpl part of the temp.
	SQLStatement statement
	(	database_connection(),
		"select name from draft_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	temp.m_dj_data->name = std8_to_wx(statement.extract<string>(0));
	SQLStatement repeater_finder
	(	database_connection(),
		"select repeater_id from repeaters where journal_id = :p"
	);
	repeater_finder.bind(":p", id());
	while (repeater_finder.step())
	{
		Id const rep_id = repeater_finder.extract<Id>(0);
		Repeater repeater(database_connection(), rep_id);
		temp.m_dj_data->repeaters.push_back(repeater);
	}
	swap(temp);
	return;
}


void
DraftJournalImpl::do_save_new()
{
	// Save the ProtoJournal part of the object
	Id const journal_id = do_save_new_journal_core(database_connection());

	// Save the derived, DraftJournalImpl part of the object
	SQLStatement statement
	(	database_connection(),
		"insert into draft_journal_detail(journal_id, name) "
		"values(:journal_id, :name)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":name", wx_to_std8(value(m_dj_data->name)));
	statement.step_final();
	
	for (Repeater& repeater: m_dj_data->repeaters)
	{
		repeater.set_journal_id(journal_id);
		repeater.save();
	}
	return;
}

void
DraftJournalImpl::do_save_existing()
{
	do_save_existing_journal_core(database_connection(), id());
	SQLStatement updater
	(	database_connection(),
		"update draft_journal_detail set name = :name where "
		"journal_id = :journal_id"
	);
	updater.bind(":journal_id", id());
	updater.bind(":name", wx_to_std8(value(m_dj_data->name)));
	updater.step_final();

	unordered_set<Id> saved_repeater_ids;
	for (Repeater& repeater: m_dj_data->repeaters)
	{
		repeater.save();
		saved_repeater_ids.insert(repeater.id());
	}
	// Now remove any repeaters in the database with this DraftJournalImpl's
	// journal_id, that no longer exist in the in-memory DraftJournalImpl
	SQLStatement repeater_finder
	(	database_connection(),
		"select repeater_id from repeaters where journal_id = :journal_id"
	);
	repeater_finder.bind(":journal_id", id());
	unordered_set<Id>::const_iterator const saved_repeaters_end =
		saved_repeater_ids.end();
	while (repeater_finder.step())
	{
		Id const repeater_id = repeater_finder.extract<Id>(0);
		if (saved_repeater_ids.find(repeater_id) == saved_repeaters_end)
		{
			// This repeater is in the database but no longer in the in-memory
			// DraftJournalImpl, and so should be deleted from the database.
			Repeater doomed_repeater(database_connection(), repeater_id);
			doomed_repeater.remove();
			// Note it's OK even if the last repeater is deleted. Another
			// repeater will never be reassigned its id - SQLite makes sure
			// of that - providing we let SQLite assign all the ids
			// automatically.
		}
	}
	return;
}


void
DraftJournalImpl::do_ghostify()
{
	do_ghostify_journal_core();
	clear(m_dj_data->name);
	for (Repeater& repeater: m_dj_data->repeaters)
	{
		repeater.ghostify();
	}
	m_dj_data->repeaters.clear();
	return;
}

void
DraftJournalImpl::do_remove()
{
	if (id() == database_connection().budget_instrument().id())
	{
		JEWEL_THROW
		(	PreservedRecordDeletionException,
			"Budget instrument DraftJournalImpl cannot be deleted."
		);
	}
	// TODO Confirm exception-safety of whole remove() function, once
	// the below is taken into account.
	SQLStatement journal_detail_deleter
	(	database_connection(),
		"delete from draft_journal_detail where journal_id = :p"
	);
	journal_detail_deleter.bind(":p", id());
	SQLStatement journal_master_deleter
	(	database_connection(),
		"delete from journals where journal_id = :p"
	);
	journal_master_deleter.bind(":p", id());
	clear_entries();
	clear_repeaters();
	save();
	journal_detail_deleter.step_final();
	journal_master_deleter.step_final();
	return;
}

bool
DraftJournalImpl::has_repeaters()
{
	load();
	return !(m_dj_data->repeaters.empty());
}

void
DraftJournalImpl::clear_repeaters()
{
	load();
	(m_dj_data->repeaters).clear();
	return;
}

void
DraftJournalImpl::clear_entries()
{
	load();
	ProtoJournal::clear_entries();
	return;
}

wxString
DraftJournalImpl::repeater_description()
{
	load();
	if (m_dj_data->repeaters.empty())
	{
		return wxString("");
	}
	JEWEL_ASSERT (!m_dj_data->repeaters.empty());
	wxString ret("This transaction is automatically recorded ");
	vector<Repeater>::const_iterator it = m_dj_data->repeaters.begin();
	ret += std8_to_wx(frequency_description(it->frequency(), "every"));
	ret += wxString(", with the next recording due on ");
	gregorian::date next_date = it->next_date();

	// TODO Make this locale-sensitive (obviously I have to make the
	// messages locale-sensitive as well in due course, but the
	// date may be harder to spot than all the English strings, so
	// flagging it explicitly with this comment).
	ret += std8_to_wx(lexical_cast<string>(next_date));

	ret += wxString(".");
	if (m_dj_data->repeaters.size() > 1)
	{
		++it;
		for ( ; it != m_dj_data->repeaters.end(); ++it)
		{
			ret += wxString
			(	"\nIn addition, this transaction is automatically recorded "
			);
			ret += std8_to_wx
			(	frequency_description(it->frequency(), "every")
			);
			ret += wxString(", with the next recording due on ");
			gregorian::date const next_date_this_cycle = it->next_date();
			
			// TODO See previous comment re. internationalization.
			ret += std8_to_wx(lexical_cast<string>(next_date_this_cycle));

			ret += wxString(".");
			if (next_date_this_cycle < next_date)
			{
				next_date = next_date_this_cycle;
			}
		}
		ret += wxString("\nThis transaction will next be recorded on ");

		// TODO See previous comment re. internationalization
		ret += wxString(lexical_cast<string>(next_date));

		ret += wxString(".");
	}
	return ret;
}	
			

void
DraftJournalImpl::mimic(ProtoJournal const& rhs)
{
	load();
	DraftJournalImpl temp(*this);
	optional<Id> t_id;
	if (temp.has_id()) t_id = temp.id();
	temp.mimic_core(rhs, database_connection(), t_id);
	swap(temp);
	return;
}


void
DraftJournalImpl::mimic(DraftJournalImpl& rhs)
{
	load();
	DraftJournalImpl temp(*this);

	// Necessary as mimic_core will only treat rhs as Journal, and
	// getters won't load.
	rhs.load();

	optional<Id> t_id;
	if (temp.has_id()) t_id = temp.id();
	temp.mimic_core(rhs, database_connection(), t_id);
	temp.set_name(rhs.name());
	temp.clear_repeaters();
	vector<Repeater> const& rreps = rhs.repeaters();
	if (!rreps.empty())
	{
		for (Repeater const& rrep: rreps)
		{
			Repeater repeater(database_connection());
			repeater.mimic(rrep);
			if (t_id) repeater.set_journal_id(value(t_id));
			temp.m_dj_data->repeaters.push_back(repeater);
		}
	}
	swap(temp);
	return;
}
			


}  // namespace phatbooks



