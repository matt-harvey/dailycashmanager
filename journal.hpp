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
#include "repeater.hpp"
#include <jewel/decimal.hpp>
#include <list>
#include <string>

namespace phatbooks
{

class Entry;


/**
 * Class to represent accounting journals. An accounting journal will
 * typically comprise two or more accounting entries, plus some
 * "journal level" data such as the date.
 */
class Journal
{
public:

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
	 * Creates an Entry and adds it to the Journal.
	 *
	 * @todo Figure out throwing behaviour. Should it check that
	 * the account exists? Etc. Etc.
	 *
	 * @param account_name name of account of added entry
	 * @param entry_comment Comment ("narration") for entry
	 * @param p_amount Amount of entry
	 */
	void add_entry
	(	std::string const& account_name,
		std::string const& entry_comment = "",
		jewel::Decimal const& p_amount = jewel::Decimal("0")
	);

	/**
	 * @returns \t true if and only if the journal is a posted journal, as
	 * opposed to a draft journal.
	 *
	 * Does not throw.
	 */
	bool is_posted() const;


private:
	bool m_is_actual;
	// if m_date == null_date(), this means it's not posted, but is a
	// draft journal (possibly autoposting).
	DateType m_date;
	std::string m_comment;
	std::list<Entry> m_entries;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp
