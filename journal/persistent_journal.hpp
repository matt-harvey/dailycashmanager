// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_persistent_journal_hpp_46241805630848654
#define GUARD_persistent_journal_hpp_46241805630848654

#include "entry.hpp"
#include "journal.hpp"
#include "phatbooks_persistent_object.hpp"
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
	typedef PhatbooksPersistentObjectBase::Id Id;
	virtual ~PersistentJournal();
};


bool
has_entry_with_id(PersistentJournal const& journal, Entry::Id entry_id);

bool
journal_id_exists(PhatbooksDatabaseConnection& dbc, PersistentJournal::Id);

PersistentJournal::Id
max_journal_id(PhatbooksDatabaseConnection& dbc);

PersistentJournal::Id
min_journal_id(PhatbooksDatabaseConnection& dbc);

bool
journal_id_is_draft(PhatbooksDatabaseConnection& dbc, PersistentJournal::Id);






}  // namespace phatbooks


#endif  // GUARD_persistent_journal_hpp_46241805630848654
