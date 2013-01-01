#ifndef GUARD_persistent_journal_hpp
#define GUARD_persistent_journal_hpp

#include "journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include "sqloxx/general_typedefs.hpp"

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
	virtual ~PersistentJournal();
	virtual void remove() = 0;
};

}  // namespace phatbooks


#endif  // GUARD_persistent_journal_hpp
