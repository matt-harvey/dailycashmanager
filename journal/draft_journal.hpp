#ifndef GUARD_draft_journal_hpp
#define GUARD_draft_journal_hpp

#include "journal.hpp"
#include <boost/optional.hpp>
#include <string>

namespace phatbooks
{

class Repeater;

class DraftJournal: public Journal
{
public:

	/**
	 * Create the tables required for the persistence
	 * of DraftJournal instances in a relational database.
	 */
	static void setup_tables(sqloxx::DatabaseConnection& dbc);

	/**
	 * Set name of DraftJournal.
	 */
	void set_name(std::string const& p_name);
	
	/**
	 * Add a Repeater to the Journal.
	 */
	void add_repeater(boost::shared_ptr<Repeater> repeater);
	
	/**
	 * @returns name of DraftJournal.
	 */
	std::string name();

private:
	
	virtual void do_load_all();

	/* WARNING Needs proper definition.
	 * This method will need to be implemented carefully
	 * (and so will do_save_existing_partial). If Entry and Repeater
	 * instances have been removed from m_entry_list and m_repeater_list
	 * since the journal was last saved, this means the corresponding
	 * entry and repeater rows will need to be deleted from the database.
	 */
	virtual void do_save_existing_all()
	{
	}

	/* WARNING Needs proper definition
	 */
	virtual void do_save_existing_partial()
	{
	}

	virtual void do_save_new_all();

	/* Note this function is not redefined here as we want
	 * it to call Journal::do_get_table_name, which returns
	 * "journal", the name of the table that controls assignment
	 * of the id to all Journal instances, regardless of derived
	 * class.
	 */
	// virtual std::string do_get_table_name();


	boost::optional<std::string> m_name;
	std::list< boost::shared_ptr<Repeater> > m_repeaters;


}  // namespace phatbooks


#endif  // GUARD_draft_journal_hpp
