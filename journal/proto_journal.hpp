#ifndef GUARD_proto_journal_hpp
#define GUARD_proto_journal_hpp

/** \file proto_journal.hpp
 *
 * \brief Header file relating to ProtoJournal class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "entry.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <jewel/decimal.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>


namespace phatbooks
{


/**
 * Class representing a Journal that has not yet taken on either
 * OrdinaryJournal or DraftJournal characteristics. This will generally
 * be a Journal that is in the process of being "constructed" by the
 * user.
 */
class ProtoJournal: public Journal
{
public:

	typedef sqloxx::Id Id;

	ProtoJournal();
	ProtoJournal(ProtoJournal const& rhs);
	// TODO Do we need a virtual destructor here? I don't think we do,
	// but confirm.
	virtual ~ProtoJournal();

	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	static std::string primary_table_name();
	static std::string primary_key_name();
	
protected:
	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	virtual void swap(ProtoJournal& rhs);

	// Other
	void do_load_journal_core
	(	PhatbooksDatabaseConnection& dbc,
		Id id
	);
	void do_save_existing_journal_core
	(	PhatbooksDatabaseConnection& dbc,
		Id id
	);
	Id do_save_new_journal_core
	(	PhatbooksDatabaseConnection& dbc
	);
	void do_ghostify_journal_core();
	

	/**
	 * Cause *this to take on the attributes of rhs that would be common
	 * to all types of Journal.
	 *
	 * Thus, for example, where rhs is an OrdinaryJournal, *this does
	 * \e not take on the \e date attribute of rhs, since ProtoJournal and
	 * DraftJournal do not have a \e date attribute.
	 * Note however that the \e id attribute is \e never taken from the
	 * rhs.
	 *
	 * The \e lhs should pass its id and database connection to the
	 * appropriate parameters in the function. The id should be wrapped
	 * in a boost::optional (uninitialized if has_id returns false).
	 *
	 * The dbc and id parameters are required in order to initialize
	 * the Entries as they are added to the lhs.
	 *
	 * Yes this is a bit messy.
	 *
	 * Note a \e deep, rather than shallow copy of the rhs Entries is made.
	 *
	 * Note this does \e not offer the strong guarantee by itself, but is
	 * designed to be called from derived classes which can implement swap
	 * etc.. to enable the strong guarantee.
	 */
	void mimic_core
	(	Journal const& rhs,
		PhatbooksDatabaseConnection& dbc,
		boost::optional<Id> id
	);

private:

	// Implement virtual functions inherited from Journal
	// todo Figure out whether these need to be virtual here.
	// I'm pretty sure they \e don't.
	virtual std::vector<Entry> const& do_get_entries() const;
	virtual void do_set_whether_actual(bool p_is_actual);
	virtual void do_set_comment(std::string const& p_comment);
	virtual void do_add_entry(Entry& entry);
	virtual std::string do_get_comment() const;
	virtual bool do_get_whether_actual() const;

	void clear_entries();

	struct ProtoJournalData
	{
		boost::optional<bool> is_actual;
		boost::optional<std::string> comment;
		std::vector<Entry> entries;
	};
	boost::scoped_ptr<ProtoJournalData> m_data;
};


ProtoJournal::Id
max_journal_id(PhatbooksDatabaseConnection& dbc);

ProtoJournal::Id
min_journal_id(PhatbooksDatabaseConnection& dbc);

bool
journal_id_exists(PhatbooksDatabaseConnection& dbc, ProtoJournal::Id);

bool
journal_id_is_draft(PhatbooksDatabaseConnection& dbc, ProtoJournal::Id);



}  // namespace phatbooks


#endif  // GUARD_proto_journal_hpp