#ifndef GUARD_persistent_journal_hpp
#define GUARD_persistent_journal_hpp

#include "journal.hpp"
#include "sqloxx/general_typedefs.hpp"

namespace phatbooks
{

/**
 * Common abstract base class for subclasses of Journal that
 * can be persisted to a database.
 */
class PersistentJournal: public Journal
{
public:
	typedef sqloxx::Id Id;

	virtual ~PersistentJournal();
	virtual Id id() const = 0;
	virtual void save() = 0;
	virtual void remove() = 0;
};

}  // namespace phatbooks


#endif  // GUARD_persistent_journal_hpp
