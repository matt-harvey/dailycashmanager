#ifndef GUARD_journal_hpp
#define GUARD_journal_hpp

/** \file journal.hpp
 *
 * \brief Header file relating to Journal class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "general_typedefs.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include <jewel/decimal.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>


namespace phatbooks
{

class Entry;

/**
 * Class to represent accounting journals.
 * An accounting journal will
 * typically comprise two or more accounting entries, plus some
 * "journal level" (as opposed to "entry level") data such as the date.
 *
 * A journal can be either an OrdinaryJournal or a DraftJournal.
 * A ordinary journal
 * has been reflected in the entity's financial state. A DraftJournal
 * has not, but has simply been saved for possible future reuse. Some
 * DraftJournal instances have got Repeater instances associated with them. A
 * DraftJournal with Repeater instances represents a recurring transaction.
 *
 * As well the ordinary/draft distinction, there is also a distinction between
 * \c actual and \c budget journals. An actual journal reflects an actual
 * change in the entity's wealth, whether the physical form of the wealth
 * (for example, by transferring between asset classes), or a dimimution
 * or augmentation in wealth (by spending or earning money). In contrast
 * a budget journal is a "conceptual" allocation or reallocation of wealth
 * in regards to the \e planned purpose to which the wealth will be put. For
 * example, allocating $100.00 of one's earnings to planned expenditure on
 * food represents a budget transaction.
 */
class Journal:
	public sqloxx::PersistentObject
{
public:

	typedef sqloxx::PersistentObject PersistentObject;
	typedef PersistentObject::Id Id;

	/**
	 * Sets up tables in the database required for the persistence of
	 * Journal objects.
	 */
	static void setup_tables(sqloxx::DatabaseConnection& dbc);

	/**
	 * Initialize a "raw" Journal, that will not yet correspond to any
	 * particular object in the database.
	 */
	explicit
	Journal
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
	);

	/**
	 * Get a Journal by id from the database.
	 */
	Journal
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
		Id p_id
	);

	virtual ~Journal();


	/**
	 * Change whether Journal is actual or budget
	 * 
	 * Does not throw.
	 */
	void set_whether_actual(bool p_is_actual);

	/**
	 * Set comment for journal
	 *
	 * Does not throw, except possibly \c std::bad_alloc in extreme
	 * circumstances.
	 */
	void set_comment(std::string const& p_comment);

	/**
	 * Add an Entry to the Journal.
	 *
	 * @todo Figure out throwing behaviour. Should it check that
	 * the account exists? Etc. Etc.
	 */
	void add_entry(boost::shared_ptr<Entry> entry);

	/**
	 * @returns true if and only if journal contains actual (as opposed to
	 * budget) transaction(s).
	 *
	 * Does not throw.
	 */
	bool is_actual();

	/**
	 * @returns journal comment.
	 *
	 * Does not throw, except perhaps \c std::bad_alloc in
	 * extreme circumstances.
	 */
	std::string comment();

	/**
	 * @returns true if and only if the journal balances, i.e. the total
	 * of the entries is equal to zero.
	 *
	 * @todo Implement it! Note, thinking a little about this function shows
	 * that all entries in a journal must be expressed in a common currency.
	 * It doesn't make sense to think of entries in a single journal as being
	 * in different currencies. An entry must have its value frozen in time.
	 */
	bool is_balanced();

	/**
	 * @returns a constant reference to the entries in the journal.
	 */
	std::vector< boost::shared_ptr<Entry> > const& entries();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(Journal& rhs);

protected:

	/**
	 * Call this function from derived classes to save a new
	 * Journal to the database, before saving the
	 * derived parts.
	 *
	 * @returns the
	 * id that will be assigned to the being-saved Journal by
	 * PersistentObject::save_new, at the end of the saving
	 * process. This avoids having to duplicate the search for
	 * the prospective id, within the derived class saving functions.
	 */
	Id do_save_new_all_journal_base();

	/**
	 * WARNING There should be a way of stopping a Journal that is not
	 * specifically either a DraftJournal or an OrdinaryJournal from
	 * being loaded from the database.
	 */
	virtual void do_load_all();

	/* WARNING Needs proper definition.
	 * This method will need to be implemented carefully
	 * (and so will do_save_existing_partial). If Entry and Repeater
	 * instances have been removed from m_entries and m_repeaters
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

	/**
	 * Copy constructor - deliberately protected.
	 */
	Journal(Journal const& rhs);

private:

	/* WARNING There should be a way of stopping a Journal that is not
	 * specifically either a DraftJournal or an OrdinaryJournal from
	 * being saved to the database.
	 */
	virtual void do_save_new_all();

	virtual std::string do_get_table_name() const;

	struct JournalData
	{
		boost::optional<bool> is_actual;
		boost::optional<std::string> comment;
		std::vector< boost::shared_ptr<Entry> > entries;
	};
	
	boost::scoped_ptr<JournalData> m_data;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp
