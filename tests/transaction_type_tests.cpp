// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_type.hpp"
#include "transaction_type.hpp"
#include <UnitTest++/UnitTest++.h>
#include <vector>

using std::vector;


#include <iostream>
using std::cout;
using std::endl;

namespace phatbooks
{
namespace test
{

typedef vector<AccountType> Vec;
typedef Vec::size_type Size;

TEST(TestSourceAccountTypesExpenditureTransaction)
{
	Vec const& account_types_ref =
		source_account_types(TransactionType::expenditure);
	Vec const account_types_cpy =
		source_account_types(TransactionType::expenditure);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(2));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK
		(	(account_types_ref[i] == AccountType::asset) ||
			(account_types_ref[i] == AccountType::liability)
		);
	}
}

TEST(TestDestinationAccountTypesExpenditureTransaction)
{
	Vec const& account_types_ref =
		destination_account_types(TransactionType::expenditure);
	Vec const account_types_cpy =
		destination_account_types(TransactionType::expenditure);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(1));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK(account_types_ref[i] == AccountType::expense);
	}
}

TEST(TestSourceAccountTypesRevenueTransaction)
{
	Vec const& account_types_ref =
		source_account_types(TransactionType::revenue);
	Vec const account_types_cpy =
		source_account_types(TransactionType::revenue);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(1));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK(account_types_ref[i] == AccountType::revenue);
	}
}

TEST(TestDestinationAccountTypesRevenueTransaction)
{
	Vec const& account_types_ref =
		destination_account_types(TransactionType::revenue);
	Vec const account_types_cpy =
		destination_account_types(TransactionType::revenue);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(2));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK
		(	(account_types_ref[i] == AccountType::asset) ||
			(account_types_ref[i] == AccountType::liability)
		);
	}
}

TEST(TestSourceAccountTypesBalanceSheetTransaction)
{
	Vec const& account_types_ref = source_account_types
	(	TransactionType::balance_sheet
	);
	Vec const account_types_cpy = source_account_types
	(	TransactionType::balance_sheet
	);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(2));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK
		(	(account_types_ref[i] == AccountType::asset) ||
			(account_types_ref[i] == AccountType::liability)
		);
	}
}

TEST(TestDestinationAccountTypesBalanceSheetTransaction)
{
	Vec const& account_types_ref = destination_account_types
	(	TransactionType::balance_sheet
	);
	Vec const account_types_cpy = destination_account_types
	(	TransactionType::balance_sheet
	);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(2));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK
		(	(account_types_ref[i] == AccountType::asset) ||
			(account_types_ref[i] == AccountType::liability)
		);
	}
}

TEST(TestSourceAccountTypesEnvelopeTransaction)
{
	Vec const& account_types_ref =
		source_account_types(TransactionType::envelope);
	Vec const account_types_cpy =
		source_account_types(TransactionType::envelope);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(3));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK
		(	(account_types_ref[i] == AccountType::revenue) ||
			(account_types_ref[i] == AccountType::expense) ||
			(account_types_ref[i] == AccountType::pure_envelope)
		);
	}
}

TEST(TestDestinationAccountTypesEnvelopeTransaction)
{
	Vec const& account_types_ref =
		destination_account_types(TransactionType::envelope);
	Vec const account_types_cpy =
		destination_account_types(TransactionType::envelope);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(3));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK
		(	(account_types_ref[i] == AccountType::revenue) ||
			(account_types_ref[i] == AccountType::expense) ||
			(account_types_ref[i] == AccountType::pure_envelope)
		);
	}
}

TEST(TestSourceAccountTypesGenericTransaction)
{
	Vec const& account_types_ref =
		source_account_types(TransactionType::generic);
	Vec const account_types_cpy =
		source_account_types(TransactionType::generic);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(6));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK
		(	(account_types_ref[i] == AccountType::revenue) ||
			(account_types_ref[i] == AccountType::expense) ||
			(account_types_ref[i] == AccountType::pure_envelope) ||
			(account_types_ref[i] == AccountType::asset) ||
			(account_types_ref[i] == AccountType::liability) ||
			(account_types_ref[i] == AccountType::equity)
		);
	}
}

TEST(TestDestinationAccountTypesGenericTransaction)
{
	Vec const& account_types_ref =
		destination_account_types(TransactionType::generic);
	Vec const account_types_cpy =
		destination_account_types(TransactionType::generic);
	CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
	CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(6));
	for (Size i = 0; i != account_types_ref.size(); ++i)
	{
		CHECK(account_types_cpy[i] == account_types_ref[i]);
		CHECK
		(	(account_types_ref[i] == AccountType::revenue) ||
			(account_types_ref[i] == AccountType::expense) ||
			(account_types_ref[i] == AccountType::pure_envelope) ||
			(account_types_ref[i] == AccountType::asset) ||
			(account_types_ref[i] == AccountType::liability) ||
			(account_types_ref[i] == AccountType::equity)
		);
	}
}

}  // namespace test
}  // namespace phatbooks
