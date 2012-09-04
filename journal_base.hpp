#ifndef GUARD_journal_base_hpp
#define GUARD_journal_base_hpp

/** \file journal_base.hpp
 *
 * \brief Header file relating to JournalBase class.
 *
 * \author Matthew Harvey
 * \date 04 Sep 2012
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "date.hpp"
#include <boost/shared_ptr.hpp>
#include <list>
#include <string>


namespace phatbooks
{

class Entry;
class Repeater;

/**
 * Abstract base class for classes representing accounting journals.
 *
 * @todo The design is currently confused about whether a Journal is a
 * necessarily posted or just a draft.
 *
 * @todo Uncomment is_balanced function when I can.
 */
class JournalBase
{
public:

	/**
	 * Change whether Journal is actual or budget
	 */
	virtual
	void set_whether_actual(bool p_is_actual) = 0;

	/**
	 * Set comment for journal
	 */
	virtual
	void set_comment(std::string const& p_comment) = 0;

	/**
	 * Set date of journal.
	 */
	virtual
	void set_date(DateType p_date) = 0;

	/**
	 * Add an Entry to the Journal.
	 */
	virtual
	void add_entry(boost::shared_ptr<Entry> entry) = 0;

	/**
	 * Add a Repeater to the Journal.
	 */
	virtual
	void add_repeater(boost::shared_ptr<Repeater> repeater) = 0;

	/**
	 * @returns \t true if and only if the journal is a posted journal, as
	 * opposed to a draft journal.
	 */
	virtual
	bool is_posted() const = 0;

	/**
	 * @returns true if and only if journal contains actual (as opposed to
	 * budget) transaction(s).
	 */
	virtual
	bool is_actual() const = 0;

	/**
	 * @returns journal date.
	 */
	virtual
	DateType date() const = 0;

	/**
	 * @returns journal comment.
	 */
	virtual
	std::string comment() const = 0;

	/**
	 * @returns true if and only if the journal balances, i.e. the total
	 * of the entries is equal to zero.
	 *
	 * @todo Make this a pure virtual, once I have implemented it in the
	 * base classes.
	virtual
	bool is_balanced() const = 0;
	*/

	/**
	 * @returns a constant reference to the list of entries in the journal.
	 */
	virtual
	std::list< boost::shared_ptr<Entry> > const& entries() const = 0;

};



}  // namespace phatbooks


#endif  // GUARD_journal_base_hpp
