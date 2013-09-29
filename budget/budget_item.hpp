// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_budget_item_hpp_0043444857021535215
#define GUARD_budget_item_hpp_0043444857021535215

#include "budget_item_impl.hpp"
#include "phatbooks_persistent_object.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <wx/string.h>
#include <ostream>
#include <vector>

namespace phatbooks
{

class Account;
class Frequency;
class PhatbooksDatabaseConnection;

/**
 * A BudgetItem represents a given item of recurring revenue or
 * expenditure, with a specific Account. For example, a
 * Christmas-related BudgetItem might recur annually, relate
 * to an Account called "gifts", and have an amount of $300.
 * BudgetItems are the "raw materials" from which
 * AmalgamatedBudgets are calculated.
 *
 * @todo Implement this.
 */
class BudgetItem:
	public PhatbooksPersistentObject<BudgetItemImpl>
{
public:
	
	typedef
		PhatbooksPersistentObject<BudgetItemImpl>
		PhatbooksPersistentObject;
	
	/**
	 * Set up tables in the database required for the persistence of
	 * BudgetItem objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Iniitialize a "raw" BudgetItem, that will not yet correspond to
	 * any particular object in the database.
	 */
	explicit BudgetItem
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Get an BudgetItem by id from the database. Throws if there is
	 * no BudgetItem with this id.
	 */
	BudgetItem
	(	PhatbooksDatabaseConnection& p_database_connection,
		sqloxx::Id p_id
	);

	BudgetItem(BudgetItem const&) = default;
	BudgetItem(BudgetItem&&) = default;
	BudgetItem& operator=(BudgetItem const&) = default;
	BudgetItem& operator=(BudgetItem&&) = default;
	~BudgetItem() = default;

	/**
	 * @returns the BudgetItem identified by \e id in the database; but
	 * does not check whether there is actually a BudgetItem with thid id.
	 * Thus it is faster than the checked constructor, but should only be
	 * used if you already know there is a BudgetItem with the given id.
	 */
	static BudgetItem create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		sqloxx::Id p_id
	);

	/**
	 * Set a description to be associated with the BudgetItem.
	 */
	void set_description(wxString const& p_description);

	/**
	 * Set the Account with which this BudgetItem will be associated.
	 * \e p_account need not have an ID when this is called; however, it
	 * should have an ID by the time the BudgetItem is first saved.
	 */
	void set_account(sqloxx::Handle<Account> const& p_account);

	/**
	 * Set the Frequency with which the BudgetItem is expected to
	 * occur.
	 */
	void set_frequency(Frequency const& p_frequency);

	/**
	 * Set the amount of the BudgetItem.
	 */
	void set_amount(jewel::Decimal const& p_amount);

	// Getters...
	wxString description() const;
	sqloxx::Handle<Account> account() const;
	Frequency frequency() const;
	jewel::Decimal amount() const;

	/**
	 * Copy attributes of rhs to *this, but do \e not copy:\n
	 * \e id, or\n
	 * \e database_connection.
	 */
	void mimic(BudgetItem const& rhs);

private:

	BudgetItem(sqloxx::Handle<BudgetItemImpl> const& p_handle);	

};


/**
 * @p_budget_items is a vector of BudgetItems which are assumed to be all
 * of the same PhatbooksDatabaseConnection and the same Account.
 *
 * @returns the amount that approximates, to the Account's native Commodity's
 * precision, the equivalent of normalizing and summing at
 * the PhatbooksDatabaseConnection's budget_frequency(), all the BudgetItems
 * in the range [b, e). Range should not be empty.
 */
jewel::Decimal
normalized_total
(	std::vector<BudgetItem>::const_iterator b,
	std::vector<BudgetItem>::const_iterator const& e
);



}  // namespace phatbooks

#endif  // GUARD_budget_item_hpp_0043444857021535215
