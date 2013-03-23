#include "budget_item.hpp"
#include "account.hpp"
#include "budget_item_impl.hpp"
#include "b_string.hpp"
#include "finformat.hpp"
#include "frequency.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include <boost/lexical_cast.hpp>
#include <jewel/decimal.hpp>
#include <jewel/output_aux.hpp>
#include <sqloxx/handle.hpp>
#include <iostream>
#include <ostream>
#include <string>

using boost::lexical_cast;
using jewel::Decimal;
using sqloxx::Handle;
using std::endl;
using std::ostream;
using std::string;

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

void
BudgetItem::output_budget_item_aux(ostream& os, BudgetItem const& bi)
{
	os << "BUDGET ITEM";
	if (bi.has_id())
	{
		// lexical cast to avoid unwanted formatting
		os << " ID " + lexical_cast<string>(bi.id());
	}
	os << ": " << finformat_std8_nopad(bi.amount()) << " ";
	os << frequency_description(bi.frequency(), "per");
	if (!bi.description().empty())
	{
		os << " (" << bstring_to_std8(bi.description()) << ")";
	}
	return;
}

ostream&
operator<<(ostream& os, BudgetItem const& bi)
{
	jewel::output_aux(os, bi, BudgetItem::output_budget_item_aux);
	return os;
}


}  // namespace phatbooks
