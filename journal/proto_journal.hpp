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


#include "consolixx/table.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <jewel/decimal.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
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
class ProtoJournal
{
public:

	typedef sqloxx::Id Id;

	ProtoJournal();
	ProtoJournal(ProtoJournal const& rhs);
	virtual ~ProtoJournal();

	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	static std::string primary_table_name();
	static std::string primary_key_name();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	virtual void swap(ProtoJournal& rhs);

	// WARNING These getters and setters are a safety concern. They are:
	// redefined in derived
	// classes. But if we don't redefine one, we land in trouble!

	// WARNING This returns a reference to internals and so
	// is a bit fucked. But client code uses it a lot...
	virtual std::vector<Entry> const& entries() const;
	virtual void set_whether_actual(bool p_is_actual);
	virtual void set_comment(std::string const& p_comment);
	virtual void add_entry(Entry& entry);
	virtual std::string comment() const;
	virtual bool is_actual() const;

	jewel::Decimal balance() const;

	/**
	 * @returns true if and only if the journal balances, i.e. the total
	 * of the entries is equal to zero.
	 *
	 * @todo Note, thinking a little about this function shows
	 * that all entries in a journal must be expressed in a common currency.
	 * It doesn't make sense to think of entries in a single journal as being
	 * in different currencies. An entry must have its value frozen in time.
	 */
	bool is_balanced() const;

protected:

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
	void clear_entries();

	/**
	 * Where J is ProtoJournal, DraftJournalImpl or OrdinaryJournalImpl,
	 * cause *this to take on the attributes of rhs that would be common
	 * to all three kinds of
	 * journal. Thus, for example, where J is OrdinaryJournal, *this does
	 * \e not take on the \e date attribute of rhs, since ProtoJournal and
	 * DraftJournal do not have a \e date attribute.
	 * Note however that the \e id attribute is \e never taken from the
	 * rhs.
	 * This does \e not offer the strong guarantee by itself.
	 *
	 * @todo
	 * // WARNING As things are in a state of flux this todo is
	 * becoming out-of-date.
	 * This is messy. The proper solution is probably as follows.
	 * Journal should be renamed to ProtoJournal.
	 * There should be a new Journal class from which ProtoJournal,
	 * DraftJournal and OrdinaryJournal inherit publically.
	 * Journal should be just an interface class with no data members.
	 * Journal should provide mimic function that takes another Journal.
	 * Common stream output code should also be factored up to Journal.
	 */
	template <typename J>
	void mimic_core(J& rhs, PhatbooksDatabaseConnection& dbc, boost::optional<Id> id);

private:

	struct ProtoJournalData;
	
	boost::scoped_ptr<ProtoJournalData> m_data;

	
};

struct ProtoJournal::ProtoJournalData
{
	boost::optional<bool> is_actual;
	boost::optional<std::string> comment;
	std::vector<Entry> entries;
};


std::ostream&
operator<<(std::ostream& os, ProtoJournal const& journal);


/**
 * J should be a Journal type (ProtoJournal, DraftJournal or OrdinaryJournal).
 */
template <typename J>
void
output_journal_aux(std::ostream& os, J const& journal);

ProtoJournal::Id
max_journal_id(PhatbooksDatabaseConnection& dbc);

ProtoJournal::Id
min_journal_id(PhatbooksDatabaseConnection& dbc);

bool
journal_id_exists(PhatbooksDatabaseConnection& dbc, ProtoJournal::Id);

bool
journal_id_is_draft(PhatbooksDatabaseConnection& dbc, ProtoJournal::Id);


template <typename J>
void
output_journal_aux(std::ostream& os, J const& journal)
{
	namespace alignment = consolixx::alignment;
	using consolixx::Table;
	using std::endl;
	using std::string;
	using std::vector;
	if (journal.is_actual()) os << "ACTUAL";
	else os << "BUDGET";
	os << endl;
	if (!journal.comment().empty()) os << journal.comment() << endl;
	os << endl;
	vector<string> headings;
	headings.push_back("ENTRY ID");
	headings.push_back("ACCOUNT");
	headings.push_back("COMMENT");
	headings.push_back("COMMODITY");
	headings.push_back("AMOUNT");
	vector<alignment::Flag> alignments(5, alignment::left);
	alignments[4] = alignment::right;
	bool const change_signs = !journal.is_actual();
	Table<Entry> const table
	(	journal.entries().begin(),
		journal.entries().end(),
		change_signs? make_reversed_entry_row: make_entry_row,
		headings,
		alignments,
		2
	);
	os << table;
	return;
}


template <typename J>
void
ProtoJournal::mimic_core(J& rhs, PhatbooksDatabaseConnection& dbc, boost::optional<Id> id)
{
	set_whether_actual(rhs.is_actual());
	set_comment(rhs.comment());
	clear_entries();
	if (!rhs.entries().empty())
	{
		for 
		(	std::vector<Entry>::const_iterator it = rhs.entries().begin(),
				end = rhs.entries().end();
			it != end;
			++it
		)
		{
			Entry entry(dbc);
			entry.mimic(*it);
			if (id) entry.set_journal_id(*id);
			add_entry(entry);
		}
	}
	return;
}



}  // namespace phatbooks


#endif  // GUARD_proto_journal_hpp
