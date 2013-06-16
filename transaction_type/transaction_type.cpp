#include "transaction_type.hpp"
#include "account.hpp"
#include "account_type.hpp"
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
	static bool initialized = false;
	if (!initialized)
	{
		size_t const sz = static_cast<size_t>(num_transaction_types);
		ret.reserve(sz);
		for (size_t i = 0; i != sz; ++i)
		{
			ret.push_back(static_cast<TransactionType>(i));
		}
		initialized = true;
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

vector<account_type::AccountType> const&
source_account_types
(	transaction_type::TransactionType p_transaction_type
)
{
	static vector<account_type::AccountType>
		ret_array[static_cast<size_t>(num_transaction_types)];
	static bool initialized = false;

	if (!initialized)
	{
#		ifndef NDEBUG
			for
			(	size_t i = 0;
				i != static_cast<size_t>(num_transaction_types);
				++i
			)
			{
				assert (ret_array[i].empty());
			}
#		endif

		ret_array[static_cast<size_t>(expenditure_transaction)].
			push_back(account_type::asset);
		ret_array[static_cast<size_t>(expenditure_transaction)].
			push_back(account_type::liability);

		ret_array[static_cast<size_t>(revenue_transaction)].
			push_back(account_type::revenue);

		ret_array[static_cast<size_t>(balance_sheet_transaction)].
			push_back(account_type::asset);
		ret_array[static_cast<size_t>(balance_sheet_transaction)].
			push_back(account_type::liability);

		ret_array[static_cast<size_t>(envelope_transaction)].
			push_back(account_type::revenue);
		ret_array[static_cast<size_t>(envelope_transaction)].
			push_back(account_type::expense);
		ret_array[static_cast<size_t>(envelope_transaction)].
			push_back(account_type::pure_envelope);

		// Generic transaction - can handle all AccountTypes;
		ret_array[static_cast<size_t>(generic_transaction)] = account_types();
	
		// Now we're initialized.
		initialized = true;
	}

#	ifndef NDEBUG
		vector<account_type::AccountType> const& debug_ret =
			ret_array[static_cast<size_t>(p_transaction_type)];
		assert (!debug_ret.empty());
		assert (initialized);
#	endif

	return ret_array[static_cast<size_t>(p_transaction_type)];
}

vector<account_type::AccountType> const&
destination_account_types
(	transaction_type::TransactionType p_transaction_type
)
{
	static vector<account_type::AccountType>
		ret_array[static_cast<size_t>(num_transaction_types)];
	static bool initialized = false;

	if (!initialized)
	{
#		ifndef NDEBUG
			for
			(	size_t i = 0;
				i != static_cast<size_t>(num_transaction_types);
				++i
			)
			{
				assert (ret_array[i].empty());
			}
#		endif

		ret_array[static_cast<size_t>(expenditure_transaction)].
			push_back(account_type::expense);

		ret_array[static_cast<size_t>(revenue_transaction)].
			push_back(account_type::asset);
		ret_array[static_cast<size_t>(revenue_transaction)].
			push_back(account_type::liability);

		ret_array[static_cast<size_t>(balance_sheet_transaction)].
			push_back(account_type::asset);
		ret_array[static_cast<size_t>(balance_sheet_transaction)].
			push_back(account_type::liability);

		ret_array[static_cast<size_t>(envelope_transaction)].
			push_back(account_type::revenue);
		ret_array[static_cast<size_t>(envelope_transaction)].
			push_back(account_type::expense);
		ret_array[static_cast<size_t>(envelope_transaction)].
			push_back(account_type::pure_envelope);

		// Generic transaction - can handle all AccountTypes;
		ret_array[static_cast<size_t>(generic_transaction)] = account_types();
	
		// Now we're initialized.
		initialized = true;
	}

#	ifndef NDEBUG
		vector<account_type::AccountType> const& debug_ret =
			ret_array[static_cast<size_t>(p_transaction_type)];
		assert (!debug_ret.empty());
		assert (initialized);
#	endif

	return ret_array[static_cast<size_t>(p_transaction_type)];
}

TransactionType
natural_transaction_type(Account const& account_x, Account const& account_y)
{
	account_type::AccountType const account_type_x = account_x.account_type();
	account_type::AccountType const account_type_y = account_y.account_type();

	switch (account_type_x)
	{
	case account_type::asset:  // fallthrough
	case account_type::liability:
		switch (account_type_y)
		{
		case account_type::asset:  // fallthrough
		case account_type::liability:
			return balance_sheet_transaction;
		case account_type::equity:
			return generic_transaction;
		case account_type::revenue:
			return revenue_transaction;
		case account_type::expense:
			return expenditure_transaction;
		case account_type::pure_envelope:
			return generic_transaction;
		default:
			assert (false);
		}
		assert (false);
	case account_type::equity:
		return generic_transaction;
	case account_type::revenue:
		switch (account_type_y)
		{
		case account_type::asset:  // fallthrough
		case account_type::liability:
			return revenue_transaction;
		case account_type::equity:
			return generic_transaction;
		case account_type::revenue:  // fallthrough
		case account_type::expense:  // fallthrough
		case account_type::pure_envelope:
			return envelope_transaction;
		default:
			assert (false);
		}
		assert (false);
	case account_type::expense:
		switch (account_type_y)
		{
		case account_type::asset:  // fallthrough
		case account_type::liability:	
			return expenditure_transaction;
		case account_type::equity:
			return generic_transaction;
		case account_type::revenue:  // fallthrough
		case account_type::expense:  // fallthrough
		case account_type::pure_envelope:
			return envelope_transaction;
		default:
			assert (false);
		}
		assert (false);
	case account_type::pure_envelope:
		return envelope_transaction;
	default:
		assert (false);
	}

	assert (false);
}

}  // namespace phatbooks
