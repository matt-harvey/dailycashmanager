// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "budget_item_handle.hpp"
#include "account.hpp"
#include "budget_item.hpp"
#include "commodity_handle.hpp"
#include "phatbooks_database_connection.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <utility>
#include <vector>

using jewel::Decimal;
using jewel::round;
using jewel::UninitializedOptionalException;
using std::move;
using std::vector;

namespace phatbooks
{

Decimal
normalized_total
(	vector<BudgetItemHandle>::const_iterator b,
	vector<BudgetItemHandle>::const_iterator const& e
)
{
	JEWEL_ASSERT (e - b > 0);  // Assert precondition.
	PhatbooksDatabaseConnection& dbc = (*b)->database_connection();
	CommodityHandle commodity(dbc);
	// WARNING Temporary hack - if Accounts can ever have Commodities other
	// than the default Commodity, then this will no longer work.
	try
	{
		commodity = (*b)->account()->commodity();
	}
	catch (jewel::UninitializedOptionalException&)
	{
		commodity = dbc.default_commodity();
	}
	Decimal::places_type const prec = commodity->precision();
	Decimal ret(0, prec);
	for ( ; b != e; ++b)
	{
		JEWEL_ASSERT
		(	(*b)->database_connection().
				supports_budget_frequency((*b)->frequency())
		);
		ret += convert_to_canonical((*b)->frequency(), (*b)->amount());
	}
	return move(round(convert_from_canonical(dbc.budget_frequency(), ret), prec));
}

}  // namespace phatbooks
