#ifndef GUARD_ordinary_journal_hpp
#define GUARD_ordinary_journal_hpp

#include "entry.hpp"
#include "journal.hpp"
#include "ordinary_journal_impl.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace phatbooks
{

class DraftJournal;
// class Journal;
class PhatbooksDatabaseConnection;

class OrdinaryJournal:
	public Journal
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

	static OrdinaryJournal create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection, 
		Id p_id
	);

	void set_whether_actual(bool p_is_actual);
	void set_comment(std::string const& p_comment);
	void set_date(boost::gregorian::date const& p_date);
	void add_entry(Entry& entry);
	bool is_actual() const;
	bool is_balanced() const;
	boost::gregorian::date date() const;
	std::string comment() const;
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

	/**
	 * Take on the attributes from \e rhs, where these exist and are
	 * applicable to OrdinaryJournal; but do \e not take on the \e id
	 * attribute of \e rhs.
	 */
	void mimic(Journal const& rhs);
	void mimic(DraftJournal const& rhs);
	void mimic(OrdinaryJournal const& rhs);

private:

	std::ostream& do_output(std::ostream& os) const;
	

	// TODO Is this even used anywhere?
	void clear_entries();



	OrdinaryJournal(sqloxx::Handle<OrdinaryJournalImpl> const& p_handle);
	sqloxx::Handle<OrdinaryJournalImpl> m_impl;
};


	



}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_hpp
