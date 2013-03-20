#include "budget_item.hpp"
#include "account.hpp"
#include "budget_item_impl.hpp"
#include "b_string.hpp"
#include "frequency.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>

using jewel::Decimal;
using sqloxx::Handle;

namespace phatbooks
{

void
BudgetItem::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	BudgetItemImpl::setup_tables(dbc);
	return;
}

BudgetItem::BudgetItem(PhatbooksDatabaseConnection& p_database_connection):
	PhatbooksPersistentObject(p_database_connection)
{
}
	
BudgetItem::BudgetItem
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
{
}

BudgetItem
BudgetItem::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return BudgetItem
	(	Handle<BudgetItemImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}

void
BudgetItem::set_description(BString const& p_description)
{
	impl().set_description(p_description);
	return;
}

void
BudgetItem::set_account(Account const& p_account)
{
	impl().set_account(p_account);
	return;
}

void
BudgetItem::set_frequency(Frequency const& p_frequency)
{
	impl().set_frequency(p_frequency);
	return;
}

void
BudgetItem::set_amount(jewel::Decimal const& p_amount)
{
	impl().set_amount(p_amount);
	return;
}

BString
BudgetItem::description() const
{
	return impl().description();
}

Account
BudgetItem::account() const
{
	return impl().account();
}

Frequency
BudgetItem::frequency() const
{
	return impl().frequency();
}

jewel::Decimal
BudgetItem::amount() const
{
	return impl().amount();
}

BudgetItem::BudgetItem(sqloxx::Handle<BudgetItemImpl> const& p_handle):
	PhatbooksPersistentObject(p_handle)
{
}


}  // namespace phatbooks
