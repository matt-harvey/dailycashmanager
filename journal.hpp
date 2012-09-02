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


#include "date.hpp"
#include "phatbooks_database_connection.hpp"
#include <jewel/decimal.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <string>


namespace phatbooks
{

class Entry;
class Repeater;


/**
 * Class to represent accounting journals. An accounting journal will
 * typically comprise two or more accounting entries, plus some
 * "journal level" data such as the date.
 */
class Journal
{
public:

	friend class sqloxx::StorageManager<Journal>;

	/**
	 * This constructor initializes journal date
	 * to null date and journal comment to empty string.
	 * The journal starts out with an empty list of entries
	 * and an empty list of repeaters, and is marked as
	 * non-posted.
	 * 
	 * @param p_is_actual determines whether this will be
	 * an "actuals" journal (as opposed to a budget journal).
	 *
	 * Does not throw.
	 */
	Journal(bool p_is_actual = true, std::string p_comment = "");

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
	bool is_posted() const;

	/**
	 * @returns true if and only if journal contains actual (as opposed to
	 * budget) transaction(s).
	 *
	 * Does not throw.
	 */
	bool is_actual() const;

	/**
	 * @returns journal date.
	 *
	 * @todo Verify throwing behaviour and determine dependence on DateType.
	 */
	DateType date() const;

	/**
	 * @returns journal comment.
	 *
	 * Does not throw, except perhaps \c std::bad_alloc in
	 * extreme circumstances.
	 */
	std::string comment() const;

	/**
	 * @returns true if and only if the journal balances, i.e. the total
	 * of the entries is equal to zero.
	 *
	 * @todo Implement it! Note, thinking a little about this function shows
	 * that all entries in a journal must be expressed in a common currency.
	 * It doesn't make sense to think of entries in a single journal as being
	 * in different currencies. An entry must have its value frozen in time.
	 */
	bool is_balanced() const;

private:
	bool m_is_actual;
	// if m_date == null_date(), this means it's not posted, but is a
	// draft journal (possibly autoposting).
	DateType m_date;
	std::string m_comment;
	std::list< boost::shared_ptr<Entry> > m_entries;
	std::list< boost::shared_ptr<Repeater> > m_repeaters;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp
