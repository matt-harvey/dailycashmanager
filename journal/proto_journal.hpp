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
#include "journal.hpp"
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
	void clear_entries();

	/**
	 * Cause *this to take on the attributes of rhs that would be common
	 * to all types of Journal.
	 * Thus, for example, where rhs is an OrdinaryJournal, *this does
	 * \e not take on the \e date attribute of rhs, since ProtoJournal and
	 * DraftJournal do not have a \e date attribute.
	 * Note however that the \e id attribute is \e never taken from the
	 * rhs.
	 * This does \e not offer the strong guarantee by itself.
	 *
	 * @todo This is sucky. Make it better.
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

	struct ProtoJournalData
	{
		boost::optional<bool> is_actual;
		boost::optional<std::string> comment;
		std::vector<Entry> entries;
	};
	boost::scoped_ptr<ProtoJournalData> m_data;
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



}  // namespace phatbooks


#endif  // GUARD_proto_journal_hpp
