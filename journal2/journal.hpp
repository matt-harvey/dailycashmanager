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


#include "sqloxx/handle.hpp"
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
class PhatbooksDatabaseConnection;

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
class Journal
{
public:

	Journal();

	typedef sqloxx::Id Id;

	/**
	 * Sets up tables in the database required for the persistence of
	 * Journal objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	virtual ~Journal();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(Journal& rhs);
	
	struct JournalData
	{
		boost::optional<bool> is_actual;
		boost::optional<std::string> comment;
		std::vector< sqloxx::Handle<Entry> > entries;
	};

	// WARNING This is fucked.
	JournalData const& data() const
	{
		return *m_data;
	}

	static std::string primary_table_name();


	// WARNING These getters and setters are problematic. They are redefined in derived
	// classes. But if we don't redefine one, we land in trouble!


	std::vector< sqloxx::Handle<Entry> > const& entries()
	{
		return m_data->entries;
	}

	void set_whether_actual(bool p_is_actual)
	{
		m_data->is_actual = p_is_actual;
		return;
	}

	void set_comment(std::string const& p_comment)
	{
		m_data->comment = p_comment;
		return;
	}

	void add_entry(sqloxx::Handle<Entry> entry)
	{
		m_data->entries.push_back(entry);
		return;
	}

		


protected:


	// WARNING This is fucked.
	boost::scoped_ptr<JournalData> m_data;


	void do_load_journal_base
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const& dbc,
		Id id
	);
	void do_save_existing_journal_base
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const& dbc,
		Id id
	);
	Id do_save_new_journal_base
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const& dbc
	);

	/**
	 * Copy constructor - deliberately unimplemented.
	 * WARNNG Get rid of this in due course.
	 */
	Journal(Journal const& rhs);

	explicit Journal(JournalData const& p_data);

	
private:

	
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp
