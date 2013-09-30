#ifndef GUARD_budget_item_data_hpp_581602907079087
#define GUARD_budget_item_data_hpp_581602907079087

#include "account_handle.hpp"
#include "budget_item.hpp"
#include "frequency.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <wx/string.h>

namespace phatbooks
{

struct BudgetItem::BudgetItemData
{
	boost::optional<AccountHandle> account;
	boost::optional<wxString> description;
	boost::optional<Frequency> frequency;
	boost::optional<jewel::Decimal> amount;
};




}  // namespace phatbooks

#endif  // GUARD_budget_item_data_hpp_581602907079087
