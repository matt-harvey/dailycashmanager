/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "draft_journal.hpp"
#include "draft_journal_table_iterator.hpp"
#include "persistent_journal.hpp"
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
#include <sqloxx/handle.hpp>
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
using sqloxx::Handle;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::unordered_set;
using std::vector;
using std::string;

namespace phatbooks
{

struct DraftJournal::DraftJournalData
{
	boost::optional<wxString> name;
	std::vector<Handle<Repeater> > repeaters;
};

string
DraftJournal::exclusive_table_name()
{
	return "draft_journal_detail";
}

vector<Handle<Repeater> > const&
DraftJournal::repeaters()
{
	load();
	return m_dj_data->repeaters;
}

void
DraftJournal::setup_tables(PhatbooksDatabaseConnection& dbc)
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

DraftJournal::DraftJournal
(	IdentityMap& p_identity_map,
	IdentityMap::Signature const& p_signature
):
	PersistentJournal(p_identity_map, p_signature),
	m_dj_data(new DraftJournalData)
{
}

DraftJournal::DraftJournal
(	IdentityMap& p_identity_map,	
	Id p_id,
	IdentityMap::Signature const& p_signature
):
	PersistentJournal(p_identity_map, p_id, p_signature),
	m_dj_data(new DraftJournalData)
{
}

DraftJournal::~DraftJournal() = default;

bool
DraftJournal::exists
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
DraftJournal::set_name(wxString const& p_name)
{
	load();
	m_dj_data->name = p_name;
	return;
}

void
DraftJournal::push_repeater(Handle<Repeater> const& repeater)
{
	load();
	if (has_id())
	{
		repeater->set_journal_id(id());
	}
	m_dj_data->repeaters.push_back(repeater);
	return;
}

wxString
DraftJournal::name()
{
	load();
	return value(m_dj_data->name);
}

DraftJournal::DraftJournal(DraftJournal const& rhs):
	PersistentJournal(rhs),
	m_dj_data(new DraftJournalData(*(rhs.m_dj_data)))
{
}

void
DraftJournal::swap(DraftJournal& rhs)
{
	PersistentJournal::swap(rhs);
	using std::swap;
	swap(m_dj_data, rhs.m_dj_data);
	return;
}

void
DraftJournal::do_load()
{
	DraftJournal temp(*this);
	
	// Load the base part of temp.
	temp.load_journal_core();

	// Load the derived, DraftJournal part of the temp.
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
		temp.m_dj_data->repeaters.push_back
		(	Handle<Repeater>(database_connection(), rep_id)
		);
	}
	swap(temp);
	return;
}

void
DraftJournal::do_save_new()
{
	// Save the PersistentJournal part of the object
	Id const journal_id = save_new_journal_core();

	// Save the derived, DraftJournal part of the object
	SQLStatement statement
	(	database_connection(),
		"insert into draft_journal_detail(journal_id, name) "
		"values(:journal_id, :name)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":name", wx_to_std8(value(m_dj_data->name)));
	statement.step_final();
	
	for (Handle<Repeater> const& repeater: m_dj_data->repeaters)
	{
		repeater->set_journal_id(journal_id);
		repeater->save();
	}
	return;
}

void
DraftJournal::do_save_existing()
{
	save_existing_journal_core();
	SQLStatement updater
	(	database_connection(),
		"update draft_journal_detail set name = :name where "
		"journal_id = :journal_id"
	);
	updater.bind(":journal_id", id());
	updater.bind(":name", wx_to_std8(value(m_dj_data->name)));
	updater.step_final();

	unordered_set<Id> saved_repeater_ids;
	for (Handle<Repeater> const& repeater: m_dj_data->repeaters)
	{
		repeater->save();
		saved_repeater_ids.insert(repeater->id());
	}
	// Now remove any repeaters in the database with this DraftJournal's
	// journal_id, that no longer exist in the in-memory DraftJournal
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
			// DraftJournal, and so should be deleted from the database.
			Handle<Repeater> const doomed_repeater(database_connection(), repeater_id);
			doomed_repeater->remove();
			// Note it's OK even if the last repeater is deleted. Another
			// repeater will never be reassigned its id - SQLite makes sure
			// of that - providing we let SQLite assign all the ids
			// automatically.
		}
	}
	return;
}

void
DraftJournal::do_ghostify()
{
	ghostify_journal_core();
	clear(m_dj_data->name);
	for (Handle<Repeater> const& repeater: m_dj_data->repeaters)
	{
		repeater->ghostify();
	}
	m_dj_data->repeaters.clear();
	return;
}

void
DraftJournal::do_remove()
{
	// Note this is wrapped in PersistentObject::remove, which
	// (a) wraps it in a DatabaseTransaction, and
	// (b) calls ghostify() if an exception is thrown.
	// This makes it exception-safe as a whole.
	if (id() == database_connection().budget_instrument()->id())
	{
		JEWEL_THROW
		(	PreservedRecordDeletionException,
			"Budget instrument DraftJournal cannot be deleted."
		);
	}
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
DraftJournal::has_repeaters()
{
	load();
	return !(m_dj_data->repeaters.empty());
}

void
DraftJournal::clear_repeaters()
{
	load();
	(m_dj_data->repeaters).clear();
	return;
}

wxString
DraftJournal::repeater_description()
{
	load();
	if (m_dj_data->repeaters.empty())
	{
		return wxString("");
	}
	JEWEL_ASSERT (!m_dj_data->repeaters.empty());
	wxString ret("This transaction is automatically recorded ");
	vector<Handle<Repeater> >::const_iterator it =
		m_dj_data->repeaters.begin();
	ret += std8_to_wx(frequency_description((*it)->frequency(), "every"));
	ret += wxString(", with the next recording due on ");
	gregorian::date next_date = (*it)->next_date();

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
			(	frequency_description((*it)->frequency(), "every")
			);
			ret += wxString(", with the next recording due on ");
			gregorian::date const next_date_this_cycle = (*it)->next_date();
			
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
DraftJournal::mimic(Journal& rhs)
{
	load();
	DraftJournal temp(*this);
	optional<Id> t_id;
	if (temp.has_id()) t_id = temp.id();
	temp.mimic_core(rhs, database_connection(), t_id);
	swap(temp);
	return;
}

void
DraftJournal::mimic(DraftJournal& rhs)
{
	load();
	DraftJournal temp(*this);

	// Necessary as mimic_core will only treat rhs as Journal, and
	// getters won't load.
	rhs.load();

	optional<Id> t_id;
	if (temp.has_id()) t_id = temp.id();
	temp.mimic_core(rhs, database_connection(), t_id);
	temp.set_name(rhs.name());
	temp.clear_repeaters();
	vector<Handle<Repeater> > const& rreps = rhs.repeaters();
	if (!rreps.empty())
	{
		for (Handle<Repeater> const& rrep: rreps)
		{
			Handle<Repeater> const repeater(database_connection());
			repeater->mimic(*rrep);
			if (t_id) repeater->set_journal_id(value(t_id));
			temp.m_dj_data->repeaters.push_back(repeater);
		}
	}
	swap(temp);
	return;
}
			


}  // namespace phatbooks



