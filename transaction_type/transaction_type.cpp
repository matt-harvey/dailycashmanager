// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "transaction_type.hpp"
#include "account.hpp"
#include "account_reader.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include <cassert>
#include <map>
#include <set>
#include <vector>

// TODO The implementations of the various functions defined in this file
// seem more complicated than they should be.

namespace phatbooks
{

using std::map;
using transaction_type::TransactionType;
using transaction_type::expenditure_transaction;
using transaction_type::revenue_transaction;
using transaction_type::balance_sheet_transaction;
using transaction_type::envelope_transaction;
using transaction_type::generic_transaction;
using transaction_type::num_transaction_types;
using std::set;
using std::vector;



// Anonymous namespace
namespace
{
	typedef set<account_type::AccountType> AccountTypeSet;

	AccountTypeSet available_account_types
	(	PhatbooksDatabaseConnection& p_database_connection
	)
	{
		AccountReader reader(p_database_connection);
		AccountTypeSet ret;
		vector<account_type::AccountType>::size_type const num_account_types =
		account_types().size();

		// Bare scope
		{
			AccountReaderBase::const_iterator it = reader.begin();
			AccountReaderBase::const_iterator const end = reader.end();
			for
			(	;   
				(it != end) &&
					(ret.size() != num_account_types);
				++it
			)
			{
				ret.insert(it->account_type());
			}
		}
		return ret;	
	}

	BString expenditure_verb()
	{
		return BString("Spend");
	}
	BString revenue_verb()
	{
		return BString("Earn");
	}
	BString balance_sheet_verb()
	{
		return BString("Account transfer");
	}
	BString envelope_verb()
	{
		return BString("Envelope transfer");
	}
	BString generic_verb()
	{
		return BString("Generic");
	}

}  // End anonymous namespace





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

vector<TransactionType>
available_transaction_types
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	typedef vector<TransactionType> TTypeVec;
	typedef vector<account_type::AccountType> ATypeVec;

	TTypeVec ret;

	AccountTypeSet const avail_account_types =
		available_account_types(p_database_connection);

	// Bare scope
	{
		TTypeVec::size_type sz = transaction_types().size();
		for (TTypeVec::size_type i = 0; i != sz; ++i)
		{
			int transaction_type_support = 0;
			AccountTypeSet::const_iterator const aset_end =
				avail_account_types.end();
			TransactionType const ttype = transaction_types()[i];

			// Bare scope
			{
				// See if has got at least one of source AccountTypes
				// for this TransactionType.
				ATypeVec::const_iterator avec_it =
					source_account_types(ttype).begin();
				ATypeVec::const_iterator const avec_end =
					source_account_types(ttype).end();
				for ( ; avec_it != avec_end; ++avec_it)
				{
					if (avail_account_types.find(*avec_it) != aset_end)
					{
						++transaction_type_support;
						break;
					}
				}
			}
			assert (transaction_type_support >= 0);
			assert (transaction_type_support <= 1);

			// Bare scope
			{
				// See if it has got at least one of the destination
				// AccountTypes for this TransactionType.
				ATypeVec::const_iterator avec_it =
					destination_account_types(ttype).begin();
				ATypeVec::const_iterator const avec_end =
					destination_account_types(ttype).end();
				for ( ; avec_it != avec_end; ++avec_it)
				{
					if (avail_account_types.find(*avec_it) != aset_end)
					{
						++transaction_type_support;
						break;
					}
				}
			}
			assert (transaction_type_support >= 0);
			assert (transaction_type_support <= 2);
		
			if (transaction_type_support == 2)
			{
				ret.push_back(ttype);
			}
		}			
	}
	return ret;
}

BString
transaction_type_to_verb(TransactionType p_transaction_type)
{
	switch (p_transaction_type)
	{
	case expenditure_transaction:
		return expenditure_verb();
	case revenue_transaction:
		return revenue_verb();
	case balance_sheet_transaction:
		return balance_sheet_verb();
	case envelope_transaction:
		return envelope_verb();
	case generic_transaction:
		return generic_verb();
	default:
		assert (false);
	}
}

TransactionType
transaction_type_from_verb(BString const& p_phrase)
{
	typedef map<BString, TransactionType> Dict;
	static Dict dict;
	static bool calculated_already = false;
	if (!calculated_already)
	{
		assert (dict.empty());
		dict[expenditure_verb()] = expenditure_transaction;
		dict[revenue_verb()] = revenue_transaction;
		dict[balance_sheet_verb()] = balance_sheet_transaction;
		dict[envelope_verb()] = envelope_transaction;
		dict[generic_verb()] = generic_transaction;
		calculated_already = true;
	}
	assert (!dict.empty());
	assert
	(	dict.size() ==
		static_cast<Dict::size_type>(num_transaction_types)
	);
	Dict::const_iterator const it = dict.find(p_phrase);
	if (it == dict.end())
	{
		throw InvalidTransactionTypeException
		(	"BString passed to transaction_type_from_verb does not "
			"correspond to any TransactionType."
		);
	}
	assert (it != dict.end());
	return it->second;
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


void
source_super_types
(	transaction_type::TransactionType p_transaction_type,
	set<account_super_type::AccountSuperType>& out
)
{
	// WARNING This is pretty inefficient. But it probably doesn't matter.
	vector<account_type::AccountType> const& atypes =
		source_account_types(p_transaction_type);
	vector<account_type::AccountType>::const_iterator it = atypes.begin();
	vector<account_type::AccountType>::const_iterator const end =
		atypes.end();
	for ( ; it != end; ++it) out.insert(super_type(*it));	
	return;
}

void
destination_super_types
(	transaction_type::TransactionType p_transaction_type,
	set<account_super_type::AccountSuperType>& out
)
{
	// WARNING This is pretty inefficient. But it probably doesn't matter.
	vector<account_type::AccountType> const& atypes =
		destination_account_types(p_transaction_type);
	vector<account_type::AccountType>::const_iterator it = atypes.begin();
	vector<account_type::AccountType>::const_iterator const end =
		atypes.end();
	for ( ; it != end; ++it) out.insert(super_type(*it));	
	return;
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
