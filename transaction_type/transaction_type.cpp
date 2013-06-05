#include "transaction_type.hpp"
#include "b_string.hpp"
#include <cassert>
#include <vector>

namespace phatbooks
{

using transaction_type::TransactionType;
using transaction_type::expenditure_transaction;
using transaction_type::revenue_transaction;
using transaction_type::balance_sheet_transaction;
using transaction_type::envelope_transaction;
using transaction_type::generic_transaction;
using transaction_type::num_transaction_types;
using std::vector;

vector<TransactionType> const&
transaction_types()
{
	static vector<TransactionType> ret;
	if (ret.empty())
	{
		size_t const sz = static_cast<size_t>(num_transaction_types);
		ret.reserve(sz);
		for (size_t i = 0; i != sz; ++i)
		{
			ret.push_back(static_cast<TransactionType>(i));
		}
	}
	assert (ret.size() == static_cast<size_t>(num_transaction_types));
	return ret;
}

BString
transaction_type_to_verb(TransactionType p_transaction_type)
{
	switch (p_transaction_type)
	{
	case expenditure_transaction:
		return "Spend";
	case revenue_transaction:
		return "Earn";
	case balance_sheet_transaction:
		return "Transfer between accounts";
	case envelope_transaction:
		return "Transfer between budget envelopes";
	case generic_transaction:
		return "Generic transaction";
	default:
		assert (false);
	}
}

bool
transaction_type_is_actual(TransactionType p_transaction_type)
{
	return p_transaction_type != envelope_transaction;
}

}  // namespace phatbooks
