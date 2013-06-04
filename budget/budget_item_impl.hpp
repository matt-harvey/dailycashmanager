// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_budget_item_impl_hpp
#define GUARD_budget_item_impl_hpp

#include "b_string.hpp"
#include "frequency.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <string>


namespace phatbooks
{


class Account;  // forward declaration


class BudgetItemImpl:
	public sqloxx::PersistentObject
	<	BudgetItemImpl,
		PhatbooksDatabaseConnection
	>
{
public:
	
	typedef
		sqloxx::PersistentObject<BudgetItemImpl, PhatbooksDatabaseConnection>
		PersistentObject;
	
	typedef typename PersistentObject::Id Id;

	typedef
		sqloxx::IdentityMap<BudgetItemImpl, PhatbooksDatabaseConnection>
		IdentityMap;
	
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	
	explicit BudgetItemImpl(IdentityMap& p_identity_map);	

	BudgetItemImpl
	(	IdentityMap& p_identity_map,
		Id p_id
	);

	~BudgetItemImpl();

	void set_description(BString const& p_description);
	void set_account(Account const& p_account);
	void set_frequency(Frequency const& p_frequency);
	void set_amount(jewel::Decimal const& p_amount);
	BString description();
	Account account();
	Frequency frequency();
	jewel::Decimal amount();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(BudgetItemImpl& rhs);

	// Keep as std::string, for consistency with sqloxx
	static std::string primary_table_name();
	static std::string exclusive_table_name();
	static std::string primary_key_name();

private:
	
	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	BudgetItemImpl(BudgetItemImpl const& rhs);

	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();
	void do_remove();
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct BudgetItemData;

	boost::scoped_ptr<BudgetItemData> m_data;
};


struct BudgetItemImpl::BudgetItemData
{
	boost::optional<Id> account_id;
	boost::optional<BString> description;
	boost::optional<Frequency> frequency;
	boost::optional<jewel::Decimal> amount;
};


}  // namespace phatbooks


#endif  // GUARD_budget_item_impl_hpp
