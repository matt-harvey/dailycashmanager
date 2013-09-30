// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_budget_item_hpp_0043444857021535215
#define GUARD_budget_item_hpp_0043444857021535215

#include "budget_item.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <vector>

namespace phatbooks
{

typedef sqloxx::Handle<BudgetItem> BudgetItemHandle;

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
(	std::vector<BudgetItemHandle>::const_iterator b,
	std::vector<BudgetItemHandle>::const_iterator const& e
);

}  // namespace phatbooks

#endif  // GUARD_budget_item_hpp_0043444857021535215
