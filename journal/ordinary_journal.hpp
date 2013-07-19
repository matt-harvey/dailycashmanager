// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_ordinary_journal_hpp
#define GUARD_ordinary_journal_hpp

#include "b_string.hpp"
#include "entry.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include "ordinary_journal_impl.hpp"
#include "proto_journal.hpp"
#include "transaction_type.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
#include <jewel/decimal.hpp>
#include <iostream>
#include <ostream>
#include <vector>

namespace phatbooks
{

class DraftJournal;
class PhatbooksDatabaseConnection;


/**
 * Represents an accounting journal that is, or will be, posted and
 * thereby reflected as a change in the economic state of the accounting
 * entity. The posting occurs when the \e save() method is called on the
 * OrdinaryJournal.
 */
class OrdinaryJournal:
	public PersistentJournal,
	virtual public PhatbooksPersistentObject<OrdinaryJournalImpl>
{
public:

	typedef
		PhatbooksPersistentObject<OrdinaryJournalImpl>
		PhatbooksPersistentObject;
	
	typedef PhatbooksPersistentObject::Id Id;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Construct a "raw" OrdinaryJournal, that will not yet correspond to
	 * any particular object in the database.
	 */
	explicit
	OrdinaryJournal
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Retrieve an OrdinaryJournal from the database by id.
	 * Throws if there is no OrdinaryJournal with this id.
	 */
	OrdinaryJournal
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * @returns an OrdinaryJournal theoretically corresponding to one that
	 * has been saved to the database with id \e p_id. However, this
	 * function does not check whether there actually is an OrdinaryJournal
	 * with this id in the database. It is the caller's responsibility to
	 * be sure there is such an OrdinaryJournal, before calling this function.
	 * This function is a faster way to get an instance of OrdinaryJournal,
	 * than by calling the (normal) constructor that takes an id.
	 */
	static OrdinaryJournal create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection, 
		Id p_id
	);

	/**
	 * @returns an OrdinaryJournal that, <em> if saved </em>, will
	 * adjust the technical opening balance of p_account to become
	 * p_desired_opening_balance. The journal will be an actual
	 * (non-budget) journal if p_account is a balance sheet Account;
	 * otherwise, if p_account is a P&L account, it will be
	 * a budget journal.
	 *
	 * The returned OrdinaryJournal should not be saved unless and until
	 * p_account has an id (i.e. is persisted to the database).
	 *
	 * Between creating the OrdinaryJournal from this
	 * function, and saving it, there should be no
	 * other adjustments made to the opening balances of
	 * Accounts.
	 */
	static OrdinaryJournal create_opening_balance_journal
	(	Account const& p_account,
		jewel::Decimal const& p_desired_opening_balance
	);

	/**
	 * Set date of OrdinaryJournal.
	 *
	 * @throws InvalidJournalDateException if the date is earlier than
	 * database_connection().entity_creation_date().
	 */
	void set_date(boost::gregorian::date const& p_date);

	/**
	 * @returns posting date.
	 */
	boost::gregorian::date date() const;

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to OrdinaryJournal; but do \e not take on the \e id of
	 * rhs (and ignore \e next_date() in the case of DraftJournal).
	 */
	void mimic(ProtoJournal const& rhs);
	void mimic(DraftJournal const& rhs);

	
private:
	
	/**
	 * Sets the date of the OrdinaryJournal. There is no restriction
	 * on what date can be passed here.
	 */
	void set_date_unrestricted(boost::gregorian::date const& p_date);

	// Define pure virtual functions inherited from Journal
	void do_set_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);
	void do_set_comment(BString const& p_comment);
	void do_set_fulcrum(size_t p_fulcrum);
	void do_push_entry(Entry& entry);
	void do_remove_entry(Entry& entry);
	void do_clear_entries();
	transaction_type::TransactionType do_get_transaction_type() const;
	BString do_get_comment() const;
	size_t do_get_fulcrum() const;
	std::vector<Entry> const& do_get_entries() const;

	// Virtual function inherited from Journal
	void do_output(std::ostream& os) const;

	OrdinaryJournal(sqloxx::Handle<OrdinaryJournalImpl> const& p_handle);
};



/**
 * Used for access control. Similar to jewel::Signature (which we can't use
 * here as we are eschewing C++11).
 */
class OrdinaryJournalSignature:
	private boost::noncopyable
{
public:
	friend class OrdinaryJournal;
private:
	OrdinaryJournalSignature()
	{
	}
};


}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_hpp
