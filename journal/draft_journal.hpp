#ifndef GUARD_draft_journal_hpp
#define GUARD_draft_journal_hpp

#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/handle.hpp"
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

namespace phatbooks
{

class DraftJournalImpl;
class Entry;
class Journal;
class PhatbooksDatabaseConnection;
class Repeater;

class DraftJournal
{
public:
	typedef sqloxx::Id Id;
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	explicit DraftJournal
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	DraftJournal
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	DraftJournal
	(	Journal& p_journal,   // TODO This should be Journal const&
		PhatbooksDatabaseConnection& p_database_connection
	);

	void set_whether_actual(bool p_is_actual);
	void set_comment(std::string const& p_comment);
	void set_name(std::string const& p_name);
	void add_entry(Entry& entry);
	void add_repeater(Repeater& repeater);
	bool is_actual() const;
	std::string comment() const;
	std::string name() const;
	bool is_balanced() const;  // TODO Implement via m_impl in due course.
	std::vector<Entry> const& entries() const;
	
	/**
	 * TODO This should eventually be shifted into a base
	 * class.
	 */
	Id id() const;

	/**
	 * TODO This should eventually be shifted into a base
	 * class.
	 */
	void save();

private:
	sqloxx::Handle<DraftJournalImpl> m_impl;


};



}  // namespace phatbooks


#endif  // GUARD_draft_journal_hpp
