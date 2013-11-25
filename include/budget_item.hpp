/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_budget_item_hpp_6804927558081656
#define GUARD_budget_item_hpp_6804927558081656

#include "dcm_database_connection.hpp"
#include <jewel/decimal_fwd.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <memory>
#include <string>
#include <vector>

namespace dcm
{

// begin forward declarations

class Account;
class Frequency;

// end forward declarations

/**
 * A BudgetItem represents a single component of the ongoing budget for a given
 * Account. Each P&L Account may have 0, 1 or more BudgetItems. For example,
 * a single BudgetItem for a "Banking fees" Account might embody an $50 credit
 * card fee that recurs annually.
 *
 * For some tests that are relevant to BudgetItem, see the tests for Account.
 *
 * NOTE, if an attempt is made to save a BudgetItem with an Account that is
 * not of AccountSuperType::pl, then
 * InvalidBudgetItemException will be thrown. This is because we do not
 * want any (non-zero) balance sheet budget amounts in the database.
 */
class BudgetItem:
	public sqloxx::PersistentObject
	<	BudgetItem,
		DcmDatabaseConnection
	>
{
public:
	
	typedef
		sqloxx::PersistentObject<BudgetItem, DcmDatabaseConnection>
		PersistentObject;

	typedef
		sqloxx::IdentityMap<BudgetItem> IdentityMap;

	/**
	 * Set up tables in the database required for the persistence of
	 * BudgetItem objects.
	 */
	static void setup_tables(DcmDatabaseConnection& dbc);

	/**
	 * Construct a "raw" BudgetItem, that will not yet be saved in the
	 * database.
	 *
	 * The Signature parameter means that this can only be called from
	 * IdentityMap. Ordinary client code should use
	 * sqloxx::Handle<BudgetItem>, not BudgetItem directly.
	 */
	BudgetItem
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	/**
	 * Get a BudgetItem by Id from the database.
	 *
	 * The Signature parameter means that this can only be called from
	 * IdentityMap. Ordinary client code should use
	 * sqloxx::Handle<BudgetItem>, not BudgetItem directly.
	 */
	BudgetItem
	(	IdentityMap& p_identity_map,
		sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);

	// copy constructor is private
	
	BudgetItem(BudgetItem&&) = delete;
	BudgetItem& operator=(BudgetItem const&) = delete;
	BudgetItem& operator=(BudgetItem&&) = delete;
	~BudgetItem();

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
	wxString description();
	sqloxx::Handle<Account> account();
	Frequency frequency();
	jewel::Decimal amount();

	// Keep as std::string, for consistency with sqloxx
	static std::string exclusive_table_name();
	static std::string primary_key_name();

	/**
	 * Copy attributes of rhs to *this, but do \e not copy:\n
	 * \e id, or\n
	 * \e database_connection.\n
	 */
	void mimic(BudgetItem& rhs);

private:
	
	void swap(BudgetItem& rhs);

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	BudgetItem(BudgetItem const& rhs);

	void do_load() override;
	void do_save_existing() override;
	void do_save_new() override;
	void do_ghostify() override;
	void do_remove() override;
	void process_saving_statement(sqloxx::SQLStatement& statement);

	/**
	 * @throws InvalidBudgetItemException if and only if the Account
	 * of the BudgetItem is not of AccountSuperType::pl.
	 */
	void ensure_pl_only_budget();

	struct BudgetItemData;

	std::unique_ptr<BudgetItemData> m_data;
};


/**
 * @p_budget_items is a vector of Handles to BudgetItems which are assumed to
 * be all of the same DcmDatabaseConnection and the same Account.
 *
 * @returns the amount that approximates, to the Account's native Commodity's
 * precision, the equivalent of normalizing and summing at
 * the DcmDatabaseConnection's budget_frequency(), all the BudgetItems
 * in the range [b, e). Range should not be empty.
 */
jewel::Decimal
normalized_total
(	std::vector<sqloxx::Handle<BudgetItem> >::const_iterator b,
	std::vector<sqloxx::Handle<BudgetItem> >::const_iterator const& e
);



}  // namespace dcm

#endif  // GUARD_budget_item_hpp_6804927558081656
