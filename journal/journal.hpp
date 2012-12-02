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
 * \e actual and \e budget journals. An actual journal reflects an actual
 * change in the entity's wealth, whether the physical form of the wealth
 * (for example, by transferring between asset classes), or a dimimution
 * or augmentation in wealth (by spending or earning money). In contrast
 * a budget journal is a "conceptual" allocation or reallocation of wealth
 * in regards to the \e planned purpose to which the wealth will be put. For
 * example, allocating $100.00 of one's earnings to planned expenditure on
 * food represents a budget transaction.
 *
 * A Journal cannot be saved to the database as \e simply a Journal. It must
 * be either specifically an OrdinaryJournal, or specifically a DraftJournal.
 * This is why Journal does not derive from PersistentObject. It does not have
 * an id() attribute. Neither does it have a database_connection() attribute.
 * The only time an object will be \e just a Journal, without being also
 * an OrdinaryJournal or a DraftJournal, will be when it is in the state
 * of being initialized (presumably by user-interfacing code) before it has
 * been saved. Prior to saving, we must convert it into either a DraftJournal
 * or an OrdinaryJournal. Despite this, the Journal class has protected
 * methods to load and save its data to a database connection that is passed
 * to the method. These are intended to be called by derived classes
 * OrdinaryJournal and DraftJournal to save or load the common base parts of
 * the object.
 *
 * @todo OrdinaryJournalImpl and DraftJournalImpl need to have remove()
 * redefined, to ensure it deletes the contained entries and repeaters
 * as well as just the journal.
 */
class Journal
{
public:

	typedef sqloxx::Id Id;

	Journal();
	Journal(Journal const& rhs);
	virtual ~Journal();

	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	static std::string primary_table_name();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	virtual void swap(Journal& rhs);

	// WARNING These getters and setters are a safety concern. They are
	// redefined in derived
	// classes. But if we don't redefine one, we land in trouble!

	// WARNING This returns a reference to internals and so
	// is a bit fucked. But client code uses it a lot...
	virtual std::vector<Entry> const& entries();

	// WARNING temp play
	void remove_first_entry();


	virtual void set_whether_actual(bool p_is_actual);
	virtual void set_comment(std::string const& p_comment);
	virtual void add_entry(Entry& entry);
	virtual std::string comment();
	virtual bool is_actual();

protected:

	void do_load_journal_base
	(	PhatbooksDatabaseConnection& dbc,
		Id id
	);
	void do_save_existing_journal_base
	(	PhatbooksDatabaseConnection& dbc,
		Id id
	);
	Id do_save_new_journal_base
	(	PhatbooksDatabaseConnection& dbc
	);
	
private:

	struct JournalData
	{
		boost::optional<bool> is_actual;
		boost::optional<std::string> comment;
		std::vector<Entry> entries;
	};

	boost::scoped_ptr<JournalData> m_data;
	
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp
