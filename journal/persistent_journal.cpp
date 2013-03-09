#include "persistent_journal.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <consolixx/table.hpp>
#include <sqloxx/sql_statement.hpp>
#include <ostream>
#include <vector>

using consolixx::Table;
using sqloxx::SQLStatement;
using std::ostream;
using std::vector;

namespace phatbooks
{
	

PersistentJournal::~PersistentJournal()
{
}

void
PersistentJournal::do_output(ostream& os) const
{
	Journal::output_core_journal_header(os);
	Table<Entry> table;
	Table<Entry>::ColumnPtr const id_column(Entry::create_id_column());
	table.push_column(id_column);
	push_core_journal_columns(table);
	table.populate(entries().begin(), entries().end());
	os << table;
	return;
}


bool
has_entry_with_id(PersistentJournal const& journal, Entry::Id entry_id)
{
	for
	(	vector<Entry>::const_iterator it = journal.entries().begin(),
			end = journal.entries().end();
		it != end;
		++it
	)
	{
		if (it->has_id() && (it->id() == entry_id))
		{
			return true;
		}
	}
	return false;
}

PersistentJournal::Id
max_journal_id(PhatbooksDatabaseConnection& dbc)
{
	SQLStatement s(dbc, "select max(journal_id) from journals");
	s.step();
	return s.extract<PersistentJournal::Id>(0);
}

PersistentJournal::Id
min_journal_id(PhatbooksDatabaseConnection& dbc)
{
	SQLStatement s(dbc, "select min(journal_id) from journals");
	s.step();
	return s.extract<PersistentJournal::Id>(0);
}

bool
journal_id_exists(PhatbooksDatabaseConnection& dbc, PersistentJournal::Id id)
{
	SQLStatement s
	(	dbc,
		"select journal_id from journals where journal_id = :p"
	);
	s.bind(":p", id);
	return s.step();
}

bool
journal_id_is_draft
(	PhatbooksDatabaseConnection& dbc,
	PersistentJournal::Id id
)
{
	if (!journal_id_exists(dbc, id))
	{
		throw std::runtime_error("Journal with id does not exist.");
	}
	SQLStatement s
	(	dbc,
		"select journal_id from draft_journal_detail where "
		"journal_id = :p"
	);
	s.bind(":p", id);
	return s.step();
}
	
}  // namespace phatbooks

