#ifndef GUARD_draft_journal_hpp
#define GUARD_draft_journal_hpp

#include "draft_journal_impl.hpp"
#include "journal.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/shared_ptr.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace phatbooks
{

class Entry;
// class Journal;
class PhatbooksDatabaseConnection;
class Repeater;

/**
 * @todo HIGH PRIORITY If the DraftJournal does not balance, we need to
 * avoid posting with autoposts. Furthermore, we need to include wording
 * in the DraftJournal description alerting the user to the fact that it
 * will not be posted for this reason.
 */
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

	static DraftJournal create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	void set_whether_actual(bool p_is_actual);
	void set_comment(std::string const& p_comment);
	void set_name(std::string const& p_name);
	void add_entry(Entry& entry);
	void add_repeater(Repeater& repeater);
	bool is_actual() const;
	bool is_balanced() const;
	std::string comment() const;
	std::string name() const;
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
	 * @returns a verbal description of the automatic postings associated
	 * with the DraftJournal (or an empty string if there are none).
	 *
	 * @todo Test.
	 */
	std::string repeater_description() const;

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to DraftJournal.
	 */
	void mimic(Journal const& rhs);

private:
	
	DraftJournal(sqloxx::Handle<DraftJournalImpl> const& p_handle);
	sqloxx::Handle<DraftJournalImpl> m_impl;

};

std::ostream&
operator<<(std::ostream& os, DraftJournal const& dj);



}  // namespace phatbooks


#endif  // GUARD_draft_journal_hpp
