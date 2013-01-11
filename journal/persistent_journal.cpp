#include "persistent_journal.hpp"
#include <vector>

using std::vector;

namespace phatbooks
{
	

PersistentJournal::~PersistentJournal()
{
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

}  // namespace phatbooks

