#ifndef GUARD_ordinary_journal_hpp
#define GUARD_ordinary_journal_hpp

#include "entry.hpp"
#include "persistent_journal.hpp"
#include "ordinary_journal_impl.hpp"
#include "proto_journal.hpp"
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
class PhatbooksDatabaseConnection;


class OrdinaryJournal: public PersistentJournal
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

	static OrdinaryJournal create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection, 
		Id p_id
	);

	void set_date(boost::gregorian::date const& p_date);
	boost::gregorian::date date() const;

	Id id() const;
	void save();
	void remove();

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to OrdinaryJournal; but do \e not take on the \e id of
	 * rhs (and ignore \e next_date() in the case of DraftJournal).
	 */
	void mimic(ProtoJournal const& rhs);
	void mimic(DraftJournal const& rhs);

private:

	// Define pure virtual functions inherited from Journal
	void do_set_whether_actual(bool p_is_actual);
	void do_set_comment(std::string const& p_comment);
	void do_add_entry(Entry& entry);
	bool do_get_whether_actual() const;
	std::string do_get_comment() const;
	std::vector<Entry> const& do_get_entries() const;

	// Redefine impure virtual function inherited from Journal
	void do_output(std::ostream& os) const;

	OrdinaryJournal(sqloxx::Handle<OrdinaryJournalImpl> const& p_handle);
	sqloxx::Handle<OrdinaryJournalImpl> m_impl;
};


	



}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_hpp
