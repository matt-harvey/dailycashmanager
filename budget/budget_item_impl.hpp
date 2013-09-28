// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_budget_item_impl_hpp_6804927558081656
#define GUARD_budget_item_impl_hpp_6804927558081656

#include "frequency.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <memory>
#include <string>


namespace phatbooks
{

// begin forward declarations

class Account;

// end forward declarations


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

	// copy constructor is private
	
	BudgetItemImpl(BudgetItemImpl&&) = delete;
	BudgetItemImpl& operator=(BudgetItemImpl const&) = delete;
	BudgetItemImpl& operator=(BudgetItemImpl&&) = delete;
	~BudgetItemImpl() = default;

	void set_description(wxString const& p_description);
	void set_account(Account const& p_account);
	void set_frequency(Frequency const& p_frequency);
	void set_amount(jewel::Decimal const& p_amount);
	wxString description();
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

	/**
	 * Copy attributes of rhs to *this, but do \e not copy:\n
	 * \e id, or\n
	 * \e database_connection.\n
	 */
	void mimic(BudgetItemImpl& rhs);

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

	std::unique_ptr<BudgetItemData> m_data;
};

// TODO We could provide a member swap function optimized for the fact that
// the m_account member is heap-allocated.
class BudgetItemImpl::BudgetItemData
{
public:
	BudgetItemData();
	~BudgetItemData();
	BudgetItemData(BudgetItemData const& rhs);
	Account account() const;
	wxString description() const;
	Frequency frequency() const;
	jewel::Decimal amount() const;
	void set_account(Account const& p_account);
	void set_description(wxString const& p_description);
	void set_frequency(Frequency const& p_frequency);
	void set_amount(jewel::Decimal const& p_amount);
	void clear();
private:
	BudgetItemData& operator=(BudgetItemData const& rhs);  // unimplemented
	Account* m_account;  // pointer, to avoid having to #include "account.hpp"
	boost::optional<wxString> m_description;
	boost::optional<Frequency> m_frequency;
	boost::optional<jewel::Decimal> m_amount;
};

}  // namespace phatbooks


#endif  // GUARD_budget_item_impl_hpp_6804927558081656
