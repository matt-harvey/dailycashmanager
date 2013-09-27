// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_impl_hpp_7344880177334361
#define GUARD_entry_impl_hpp_7344880177334361

/** \file entry.hpp
 *
 * \brief Header file pertaining to EntryImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "account.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_side.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <wx/string.h>
#include <memory>
#include <string>


namespace phatbooks
{



/**
 * Provides implementation for Entry. Multiple Entry instances may share the
 * same EntryImpl, i.e. when they are referring to the same underlying
 * persistent object i.e. record or would-be-record in the database.
 */
class EntryImpl:
	public sqloxx::PersistentObject<EntryImpl, PhatbooksDatabaseConnection>
{
public:

	typedef sqloxx::PersistentObject<EntryImpl, PhatbooksDatabaseConnection>
		PersistentObject;

	typedef typename PersistentObject::Id Id;

	typedef sqloxx::IdentityMap<EntryImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	
	explicit
	EntryImpl(IdentityMap& p_identity_map);

	EntryImpl
	(	IdentityMap& p_identity_map,	
		Id p_id
	);

	~EntryImpl();

	void set_journal_id(Id p_journal_id);

	void set_account(Account const& p_account);

	void set_comment(wxString const& p_comment);

	void set_amount(jewel::Decimal const& p_amount);
	
	void set_whether_reconciled(bool p_is_reconciled);

	void set_transaction_side
	(	transaction_side::TransactionSide p_transaction_side
	);

	/**
	 * Does not throw except possibly \c std::bad_alloc in
	 * extreme circumstances.
	 */
	wxString comment();

	/**
	 * @returns EntryImpl amount (+ve for debits, -ve for credits).
	 *
	 * @todo Verify throwing behaviour.
	 */
	jewel::Decimal amount();

	Account account();

	bool is_reconciled();

	transaction_side::TransactionSide transaction_side();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(EntryImpl& rhs);

	// Keep as std::string, for consistency with sqloxx
	static std::string primary_table_name();
	static std::string exclusive_table_name();
	static std::string primary_key_name();

	/**
	 * Copy attributes of rhs to *this, but do \e not copy:\n
	 * \e id,\n
	 * \e database_connection, or \n
	 * \e journal_id.
	 */
	void mimic(EntryImpl& rhs);

	template <typename PersistentJournalType>
	PersistentJournalType journal();

private:

	/**
	 * Copy constructor - implemented, but deliberately private
	 */
	EntryImpl(EntryImpl const& rhs);
	
	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();
	void do_remove();
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct EntryData;

	boost::scoped_ptr<EntryData> m_data;

};


struct EntryImpl::EntryData
{
	boost::optional<Id> journal_id;
	boost::optional<Account> account;
	boost::optional<wxString> comment;
	boost::optional<jewel::Decimal> amount;
	boost::optional<bool> is_reconciled;
	boost::optional<transaction_side::TransactionSide> transaction_side;
};


template <typename PersistentJournalType>
PersistentJournalType
EntryImpl::journal()
{
	load();
	return PersistentJournalType
	(	database_connection(),
		jewel::value(m_data->journal_id)
	);
}


/**
 * @returns a unique_ptr to a heap-allocated SQLStatement from which
 * entry_id may be selected from the first result column. Contains
 * only Entries that belong to actual (i.e. non-budget)
 * Journals that are OrdinaryJournals (i.e. not DraftJournals).
 * Filtering may optionally be performed by Account and/or date.
 */
std::unique_ptr<sqloxx::SQLStatement>
create_date_ordered_actual_ordinary_entry_selector_aux
(	PhatbooksDatabaseConnection& p_database_connection,
	boost::optional<boost::gregorian::date> const& p_maybe_min_date =
		boost::optional<boost::gregorian::date>(),
	boost::optional<boost::gregorian::date> const& p_maybe_max_date =
		boost::optional<boost::gregorian::date>(),
	boost::optional<Account> const& p_maybe_account =
		boost::optional<Account>()
);

}  // namespace phatbooks

#endif  // GUARD_entry_impl_hpp_7344880177334361
