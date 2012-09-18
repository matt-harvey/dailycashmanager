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
#include "date.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include <jewel/decimal.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <string>


namespace phatbooks
{

class Entry;
class Repeater;

/**
 * Class to represent accounting journals.
 * An accounting journal will
 * typically comprise two or more accounting entries, plus some
 * "journal level" data such as the date.
 *
 * A journal can be either \e posted or \e draft. A posted journal
 * has been reflected in the entity's financial state. A draft journal
 * has not, but has simply been saved for possible future reuse. Some
 * draft journals have got Repeater instances associated with them. A
 * draft journal with Repeater instances constitutes a recurring transaction.
 *
 * As well the posted/draft distinction, there is also a distinction between
 * \c actual and \c budget journals. An actual journal reflects an actual
 * change in the entity's wealth, whether the physical form of the wealth
 * (for example, by transferring between asset classes), or a dimimution
 * or augmentation in wealth (by spending or earning money). In contrast
 * a budget journal is a "conceptual" allocation or reallocation of wealth
 * in regards to the \e planned purpose to which the wealth will be put. Thus,
 * allocating $100.00 of one's earnings to planned expenditure on food
 * represents a budget transaction.
 *
 * @todo The do_save... methods of Journal do not currently store the contents
 * of the Repeater list anywhere. The contents should be stored in the
 * repeaters table in the database.
 *
 * @todo The Entry and Repeater classes are currently being saved and loaded
 * as part of the code in Journal. But what will happen when we want to modify
 * them? How will we know where to save them? They should probably be
 * PersistentObject instances in their own right.
 */
class Journal: public sqloxx::PersistentObject<IdType>
{
public:

	typedef IdType Id;
	typedef sqloxx::PersistentObject<Id> PersistentObject;

	/**
	 * Sets up tables in the database required for the persistence of
	 * Journal objects.
	 */
	static void setup_tables(sqloxx::DatabaseConnection& dbc);

	/**
	 * Initialize a "draft" journal, that will not correspond to any
	 * particular object in the database ("draft" here means not-yet-persisted,
	 * as opposed to not-yet-posted).
	 *
	 * @todo Make this documentation clearer.
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
	 * Set date of journal.
	 *
	 * Does not throw.
	 */
	void set_date(boost::gregorian::date const& p_date);

	/**
	 * Add an Entry to the Journal.
	 *
	 * @todo Figure out throwing behaviour. Should it check that
	 * the account exists? Etc. Etc.
	 */
	void add_entry(boost::shared_ptr<Entry> entry);

	/**
	 * Add a Repeater to the Journal.
	 *
	 * @todo figure out throwing behaviour.
	 */
	void add_repeater(boost::shared_ptr<Repeater> repeater);

	/**
	 * @returns \t true if and only if the journal is a posted journal, as
	 * opposed to a draft journal.
	 *
	 * Does not throw.
	 */
	bool is_posted();

	/**
	 * @returns true if and only if journal contains actual (as opposed to
	 * budget) transaction(s).
	 *
	 * Does not throw.
	 */
	bool is_actual();

	/**
	 * @returns journal date.
	 *
	 * @todo Verify throwing behaviour and determine dependence on DateRep.
	 */
	boost::gregorian::date date();

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
	 * @returns a constant reference to the list of entries in the journal.
	 */
	std::list< boost::shared_ptr<Entry> > const& entries();

private:

	virtual void do_load_all();

	/* WARNING Needs proper definition.
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

	virtual std::string do_get_table_name();



	boost::optional<bool> m_is_actual;
	// if m_date == null_date(), this means it's not posted, but is a
	// draft journal (possibly autoposting).
	boost::optional<DateRep> m_date;
	boost::optional<std::string> m_comment;
	std::list< boost::shared_ptr<Entry> > m_entries;
	std::list< boost::shared_ptr<Repeater> > m_repeaters;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp
