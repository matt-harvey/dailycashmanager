// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_persistent_journal_hpp_46241805630848654
#define GUARD_persistent_journal_hpp_46241805630848654

#include "entry_handle.hpp"
#include "journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <ostream>

namespace phatbooks
{

/**
 * Common abstract base class for subclasses of Journal that
 * can be persisted to a database.
 */
class PersistentJournal:
	public Journal,
	virtual public PhatbooksPersistentObjectBase
{
public:
	PersistentJournal() = default;
	PersistentJournal(PersistentJournal const&) = default;
	PersistentJournal(PersistentJournal&&) = default;
	PersistentJournal& operator=(PersistentJournal const&) = default;
	PersistentJournal& operator=(PersistentJournal&&) = default;
	virtual ~PersistentJournal() = default;
};


bool
has_entry_with_id(PersistentJournal const& journal, sqloxx::Id entry_id);

bool
journal_id_exists(PhatbooksDatabaseConnection& dbc, sqloxx::Id);

sqloxx::Id
max_journal_id(PhatbooksDatabaseConnection& dbc);

sqloxx::Id
min_journal_id(PhatbooksDatabaseConnection& dbc);

bool
journal_id_is_draft(PhatbooksDatabaseConnection& dbc, sqloxx::Id);






}  // namespace phatbooks


#endif  // GUARD_persistent_journal_hpp_46241805630848654
