#ifndef GUARD_ordinary_journal_hpp
#define GUARD_ordinary_journal_hpp

#include "ordinary_journal_reader.hpp"
#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/handle.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <string>

namespace phatbooks
{

class Entry;
class Journal;
class OrdinaryJournalImpl;
class PhatbooksDatabaseConnection;

class OrdinaryJournal
{
public:

	typedef sqloxx::Id Id;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	explicit
	OrdinaryJournal
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	OrdinaryJournal
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	OrdinaryJournal
	(	Journal& p_journal,  // TODO This should be Journal const&
		PhatbooksDatabaseConnection& p_database_connection
	);

	explicit
	OrdinaryJournal(OrdinaryJournalReader const& p_reader);

	void set_whether_actual(bool p_is_actual);
	void set_comment(std::string const& p_comment);
	void set_date(boost::gregorian::date const& p_date);
	void add_entry(Entry& entry);
	bool is_actual() const;
	boost::gregorian::date date() const;
	std::string comment() const;
	bool is_balanced() const;  // TODO Implement (by delegating to m_impl)
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
	
	// WARNING temp play
	void remove_first_entry();

private:
	sqloxx::Handle<OrdinaryJournalImpl> m_impl;




};


}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_hpp
