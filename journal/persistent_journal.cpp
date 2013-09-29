// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "persistent_journal.hpp"
#include "entry_handle.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/sql_statement.hpp>
#include <ostream>
#include <vector>

using sqloxx::Id;
using sqloxx::SQLStatement;
using std::ostream;
using std::vector;

namespace phatbooks
{
	

bool
has_entry_with_id(PersistentJournal const& journal, Id entry_id)
{
	for (EntryHandle const& entry: journal.entries())
	{
		if (entry->has_id() && (entry->id() == entry_id))
		{
			return true;
		}
	}
	return false;
}

Id
max_journal_id(PhatbooksDatabaseConnection& dbc)
{
	SQLStatement s(dbc, "select max(journal_id) from journals");
	s.step();
	return s.extract<Id>(0);
}

Id
min_journal_id(PhatbooksDatabaseConnection& dbc)
{
	SQLStatement s(dbc, "select min(journal_id) from journals");
	s.step();
	return s.extract<Id>(0);
}

bool
journal_id_exists(PhatbooksDatabaseConnection& dbc, Id id)
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
	Id id
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

