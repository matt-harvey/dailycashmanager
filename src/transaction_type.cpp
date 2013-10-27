/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "transaction_type.hpp"
#include "account.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "date.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/string.h>
#include <map>
#include <set>
#include <vector>

// TODO LOW PRIORITY The implementations of the various functions defined in
// this file seem more complicated than they should be.

namespace gregorian = boost::gregorian;

namespace phatbooks
{

using std::map;
using sqloxx::Handle;
using sqloxx::SQLStatement;
using std::set;
using std::vector;

// Anonymous namespace
namespace
{
	typedef set<AccountType> AccountTypeSet;

	AccountTypeSet available_account_types
	(	PhatbooksDatabaseConnection& p_database_connection
	)
	{
		AccountTypeSet ret;
		vector<AccountType>::size_type const num_account_types =
			account_types().size();
		AccountTableIterator it(p_database_connection);
		AccountTableIterator const end;
		for ( ; it != end; ++it)
		{
			ret.insert((*it)->account_type());
			if (ret.size() == num_account_types)
			{
				break;
			}
		}
		return ret;	
	}

	wxString expenditure_verb()
	{
		return wxString("Spend");
	}
	wxString revenue_verb()
	{
		return wxString("Earn");
	}
	wxString balance_sheet_verb()
	{
		return wxString("Account transfer");
	}
	wxString envelope_verb()
	{
		return wxString("Envelope transfer");
	}
	wxString generic_verb()
	{
		return wxString("Generic");
	}

}  // End anonymous namespace





vector<TransactionType> const&
transaction_types()
{
	static vector<TransactionType> ret;
	static bool initialized = false;
	if (!initialized)
	{
		size_t const sz =
			static_cast<size_t>(TransactionType::num_transaction_types);
		ret.reserve(sz);
		for (size_t i = 0; i != sz; ++i)
		{
			ret.push_back(static_cast<TransactionType>(i));
		}
		initialized = true;
	}
	JEWEL_ASSERT
	(	ret.size() ==
		static_cast<size_t>(TransactionType::num_transaction_types)
	);
	return ret;
}

vector<TransactionType>
available_transaction_types
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	typedef vector<TransactionType> TTypeVec;
	typedef vector<AccountType> ATypeVec;

	TTypeVec ret;

	AccountTypeSet const avail_account_types =
		available_account_types(p_database_connection);

	TTypeVec const& ttypes = transaction_types();
	for (TransactionType const ttype: ttypes)
	{
		int transaction_type_support = 0;
		AccountTypeSet::const_iterator const aset_end =
			avail_account_types.end();

		// See if has got at least one of source AccountTypes
		// for this TransactionType.
		ATypeVec const& savec = source_account_types(ttype);
		for (AccountType const atype: savec)
		{
			if (avail_account_types.find(atype) != aset_end)
			{
				++transaction_type_support;
				break;
			}
		}
		JEWEL_ASSERT (transaction_type_support >= 0);
		JEWEL_ASSERT (transaction_type_support <= 1);

		// See if it has got at least one of the destination
		// AccountTypes for this TransactionType.
		ATypeVec const& davec = destination_account_types(ttype);
		for (AccountType const atype: davec)
		{
			if (avail_account_types.find(atype) != aset_end)
			{
				++transaction_type_support;
				break;
			}
		}
		JEWEL_ASSERT (transaction_type_support >= 0);
		JEWEL_ASSERT (transaction_type_support <= 2);
	
		if (transaction_type_support == 2)
		{
			ret.push_back(ttype);
		}
	}			
	return ret;
}

wxString
transaction_type_to_verb(TransactionType p_transaction_type)
{
	switch (p_transaction_type)
	{
	case TransactionType::expenditure:
		return expenditure_verb();
	case TransactionType::revenue:
		return revenue_verb();
	case TransactionType::balance_sheet:
		return balance_sheet_verb();
	case TransactionType::envelope:
		return envelope_verb();
	case TransactionType::generic:
		return generic_verb();
	default:
		JEWEL_HARD_ASSERT (false);
	}
}

TransactionType
transaction_type_from_verb(wxString const& p_phrase)
{
	typedef map<wxString, TransactionType> Dict;
	static Dict dict;
	static bool calculated_already = false;
	if (!calculated_already)
	{
		JEWEL_ASSERT (dict.empty());
		dict[expenditure_verb()] = TransactionType::expenditure;
		dict[revenue_verb()] = TransactionType::revenue;
		dict[balance_sheet_verb()] = TransactionType::balance_sheet;
		dict[envelope_verb()] = TransactionType::envelope;
		dict[generic_verb()] = TransactionType::generic;
		calculated_already = true;
	}
	JEWEL_ASSERT (!dict.empty());
	JEWEL_ASSERT
	(	dict.size() ==
		static_cast<Dict::size_type>(TransactionType::num_transaction_types)
	);
	Dict::const_iterator const it = dict.find(p_phrase);
	if (it == dict.end())
	{
		JEWEL_THROW
		(	InvalidTransactionTypeException,
			"wxString passed to transaction_type_from_verb does not "
			"correspond to any TransactionType."
		);
	}
	JEWEL_ASSERT (it != dict.end());
	return it->second;
}

bool
transaction_type_is_actual(TransactionType p_transaction_type)
{
	return p_transaction_type != TransactionType::envelope;
}

TransactionType
non_actual_transaction_type()
{
	TransactionType const ret = TransactionType::envelope;
#	ifndef NDEBUG
		int const lim =
			static_cast<int>(TransactionType::num_transaction_types);
		for (int i = 0 ; i != lim; ++i)
		{
			TransactionType const ttype = static_cast<TransactionType>(i);
			if (ttype == ret)
			{
				JEWEL_ASSERT (!transaction_type_is_actual(ttype));
			}
			else
			{
				JEWEL_ASSERT (transaction_type_is_actual(ttype));
			}
		}
#	endif  // NDEBUG
	return ret;
}

vector<AccountType> const&
source_account_types
(	TransactionType p_transaction_type
)
{
	static vector<AccountType>
		ret_array[static_cast<size_t>(TransactionType::num_transaction_types)];
	static bool initialized = false;

	if (!initialized)
	{
		try
		{
	#		ifndef NDEBUG
				size_t const sz =
					static_cast<size_t>(TransactionType::num_transaction_types);
				for (size_t i = 0; i != sz; ++i)
				{
					JEWEL_ASSERT (ret_array[i].empty());
				}
	#		endif

			ret_array[static_cast<size_t>(TransactionType::expenditure)].
				push_back(AccountType::asset);
			ret_array[static_cast<size_t>(TransactionType::expenditure)].
				push_back(AccountType::liability);

			ret_array[static_cast<size_t>(TransactionType::revenue)].
				push_back(AccountType::revenue);

			ret_array[static_cast<size_t>(TransactionType::balance_sheet)].
				push_back(AccountType::asset);
			ret_array[static_cast<size_t>(TransactionType::balance_sheet)].
				push_back(AccountType::liability);

			ret_array[static_cast<size_t>(TransactionType::envelope)].
				push_back(AccountType::revenue);
			ret_array[static_cast<size_t>(TransactionType::envelope)].
				push_back(AccountType::expense);
			ret_array[static_cast<size_t>(TransactionType::envelope)].
				push_back(AccountType::pure_envelope);

			// Generic transaction - can handle all AccountTypes;
			ret_array[static_cast<size_t>(TransactionType::generic)] =
				account_types();
		}
		catch (...)
		{
			for (auto& vec: ret_array) vec.clear();
			throw;
		}
	
		// Now we're initialized.
		initialized = true;
	}

#	ifndef NDEBUG
		vector<AccountType> const& debug_ret =
			ret_array[static_cast<size_t>(p_transaction_type)];
		JEWEL_ASSERT (!debug_ret.empty());
		JEWEL_ASSERT (initialized);
#	endif

	return ret_array[static_cast<size_t>(p_transaction_type)];
}


void
source_super_types
(	TransactionType p_transaction_type,
	set<AccountSuperType>& out
)
{
	vector<AccountType> const& atypes =
		source_account_types(p_transaction_type);
	for (AccountType const atype: atypes)
	{
		out.insert(super_type(atype));
	}
	return;
}

void
destination_super_types
(	TransactionType p_transaction_type,
	set<AccountSuperType>& out
)
{
	vector<AccountType> const& atypes =
		destination_account_types(p_transaction_type);
	for (AccountType const atype: atypes)
	{
		out.insert(super_type(atype));
	}
	return;
}

vector<AccountType> const&
destination_account_types
(	TransactionType p_transaction_type
)
{
	static vector<AccountType>
		ret_array[static_cast<size_t>(TransactionType::num_transaction_types)];
	static bool initialized = false;

	if (!initialized)
	{
#		ifndef NDEBUG
			for
			(	size_t i = 0;
				i != static_cast<size_t>(TransactionType::num_transaction_types);
				++i
			)
			{
				JEWEL_ASSERT (ret_array[i].empty());
			}
#		endif

		ret_array[static_cast<size_t>(TransactionType::expenditure)].
			push_back(AccountType::expense);

		ret_array[static_cast<size_t>(TransactionType::revenue)].
			push_back(AccountType::asset);
		ret_array[static_cast<size_t>(TransactionType::revenue)].
			push_back(AccountType::liability);

		ret_array[static_cast<size_t>(TransactionType::balance_sheet)].
			push_back(AccountType::asset);
		ret_array[static_cast<size_t>(TransactionType::balance_sheet)].
			push_back(AccountType::liability);

		ret_array[static_cast<size_t>(TransactionType::envelope)].
			push_back(AccountType::revenue);
		ret_array[static_cast<size_t>(TransactionType::envelope)].
			push_back(AccountType::expense);
		ret_array[static_cast<size_t>(TransactionType::envelope)].
			push_back(AccountType::pure_envelope);

		// Generic transaction - can handle all AccountTypes;
		ret_array[static_cast<size_t>(TransactionType::generic)] = account_types();
	
		// Now we're initialized.
		initialized = true;
	}

#	ifndef NDEBUG
		vector<AccountType> const& debug_ret =
			ret_array[static_cast<size_t>(p_transaction_type)];
		JEWEL_ASSERT (!debug_ret.empty());
		JEWEL_ASSERT (initialized);
#	endif

	return ret_array[static_cast<size_t>(p_transaction_type)];
}

TransactionType
natural_transaction_type
(	Handle<Account> const& account_x,
	Handle<Account> const& account_y
)
{
	AccountType const account_type_x = account_x->account_type();
	AccountType const account_type_y = account_y->account_type();

	switch (account_type_x)
	{
	case AccountType::asset:  // fallthrough
	case AccountType::liability:
		switch (account_type_y)
		{
		case AccountType::asset:  // fallthrough
		case AccountType::liability:
			return TransactionType::balance_sheet;
		case AccountType::equity:
			return TransactionType::generic;
		case AccountType::revenue:
			return TransactionType::revenue;
		case AccountType::expense:
			return TransactionType::expenditure;
		case AccountType::pure_envelope:
			return TransactionType::generic;
		default:
			JEWEL_HARD_ASSERT (false);
		}
		JEWEL_HARD_ASSERT (false);
	case AccountType::equity:
		return TransactionType::generic;
	case AccountType::revenue:
		switch (account_type_y)
		{
		case AccountType::asset:  // fallthrough
		case AccountType::liability:
			return TransactionType::revenue;
		case AccountType::equity:
			return TransactionType::generic;
		case AccountType::revenue:  // fallthrough
		case AccountType::expense:  // fallthrough
		case AccountType::pure_envelope:
			return TransactionType::envelope;
		default:
			JEWEL_HARD_ASSERT (false);
		}
		JEWEL_HARD_ASSERT (false);
	case AccountType::expense:
		switch (account_type_y)
		{
		case AccountType::asset:  // fallthrough
		case AccountType::liability:	
			return TransactionType::expenditure;
		case AccountType::equity:
			return TransactionType::generic;
		case AccountType::revenue:  // fallthrough
		case AccountType::expense:  // fallthrough
		case AccountType::pure_envelope:
			return TransactionType::envelope;
		default:
			JEWEL_HARD_ASSERT (false);
		}
		JEWEL_HARD_ASSERT (false);
	case AccountType::pure_envelope:
		return TransactionType::envelope;
	default:
		JEWEL_HARD_ASSERT (false);
	}

	JEWEL_HARD_ASSERT (false);
}


}  // namespace phatbooks
