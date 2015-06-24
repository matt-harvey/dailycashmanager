/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "account_type.hpp"
#include "transaction_type.hpp"
#include <boost/test/unit_test.hpp>
#include <vector>

using std::vector;


#include <iostream>
using std::cout;
using std::endl;

namespace dcm
{
namespace test
{

using Vec =  vector<AccountType>;
using Size = Vec::size_type;

BOOST_AUTO_TEST_CASE(TestSourceAccountTypesExpenditureTransaction)
{
    Vec const& account_types_ref =
        source_account_types(TransactionType::expenditure);
    Vec const account_types_cpy =
        source_account_types(TransactionType::expenditure);
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(2));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK
        (   (account_types_ref[i] == AccountType::asset) ||
            (account_types_ref[i] == AccountType::liability)
        );
    }
}

BOOST_AUTO_TEST_CASE(TestDestinationAccountTypesExpenditureTransaction)
{
    Vec const& account_types_ref =
        destination_account_types(TransactionType::expenditure);
    Vec const account_types_cpy =
        destination_account_types(TransactionType::expenditure);
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(1));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK(account_types_ref[i] == AccountType::expense);
    }
}

BOOST_AUTO_TEST_CASE(TestSourceAccountTypesRevenueTransaction)
{
    Vec const& account_types_ref =
        source_account_types(TransactionType::revenue);
    Vec const account_types_cpy =
        source_account_types(TransactionType::revenue);
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(1));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK(account_types_ref[i] == AccountType::revenue);
    }
}

BOOST_AUTO_TEST_CASE(TestDestinationAccountTypesRevenueTransaction)
{
    Vec const& account_types_ref =
        destination_account_types(TransactionType::revenue);
    Vec const account_types_cpy =
        destination_account_types(TransactionType::revenue);
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(2));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK
        (   (account_types_ref[i] == AccountType::asset) ||
            (account_types_ref[i] == AccountType::liability)
        );
    }
}

BOOST_AUTO_TEST_CASE(TestSourceAccountTypesBalanceSheetTransaction)
{
    Vec const& account_types_ref = source_account_types
    (   TransactionType::balance_sheet
    );
    Vec const account_types_cpy = source_account_types
    (   TransactionType::balance_sheet
    );
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(2));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK
        (   (account_types_ref[i] == AccountType::asset) ||
            (account_types_ref[i] == AccountType::liability)
        );
    }
}

BOOST_AUTO_TEST_CASE(TestDestinationAccountTypesBalanceSheetTransaction)
{
    Vec const& account_types_ref = destination_account_types
    (   TransactionType::balance_sheet
    );
    Vec const account_types_cpy = destination_account_types
    (   TransactionType::balance_sheet
    );
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(2));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK
        (   (account_types_ref[i] == AccountType::asset) ||
            (account_types_ref[i] == AccountType::liability)
        );
    }
}

BOOST_AUTO_TEST_CASE(TestSourceAccountTypesEnvelopeTransaction)
{
    Vec const& account_types_ref =
        source_account_types(TransactionType::envelope);
    Vec const account_types_cpy =
        source_account_types(TransactionType::envelope);
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(3));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK
        (   (account_types_ref[i] == AccountType::revenue) ||
            (account_types_ref[i] == AccountType::expense) ||
            (account_types_ref[i] == AccountType::pure_envelope)
        );
    }
}

BOOST_AUTO_TEST_CASE(TestDestinationAccountTypesEnvelopeTransaction)
{
    Vec const& account_types_ref =
        destination_account_types(TransactionType::envelope);
    Vec const account_types_cpy =
        destination_account_types(TransactionType::envelope);
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(3));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK
        (   (account_types_ref[i] == AccountType::revenue) ||
            (account_types_ref[i] == AccountType::expense) ||
            (account_types_ref[i] == AccountType::pure_envelope)
        );
    }
}

BOOST_AUTO_TEST_CASE(TestSourceAccountTypesGenericTransaction)
{
    Vec const& account_types_ref =
        source_account_types(TransactionType::generic);
    Vec const account_types_cpy =
        source_account_types(TransactionType::generic);
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(6));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK
        (   (account_types_ref[i] == AccountType::revenue) ||
            (account_types_ref[i] == AccountType::expense) ||
            (account_types_ref[i] == AccountType::pure_envelope) ||
            (account_types_ref[i] == AccountType::asset) ||
            (account_types_ref[i] == AccountType::liability) ||
            (account_types_ref[i] == AccountType::equity)
        );
    }
}

BOOST_AUTO_TEST_CASE(TestDestinationAccountTypesGenericTransaction)
{
    Vec const& account_types_ref =
        destination_account_types(TransactionType::generic);
    Vec const account_types_cpy =
        destination_account_types(TransactionType::generic);
    BOOST_CHECK_EQUAL(account_types_ref.size(), account_types_cpy.size());
    BOOST_CHECK_EQUAL(account_types_ref.size(), static_cast<size_t>(6));
    for (Size i = 0; i != account_types_ref.size(); ++i)
    {
        BOOST_CHECK(account_types_cpy[i] == account_types_ref[i]);
        BOOST_CHECK
        (   (account_types_ref[i] == AccountType::revenue) ||
            (account_types_ref[i] == AccountType::expense) ||
            (account_types_ref[i] == AccountType::pure_envelope) ||
            (account_types_ref[i] == AccountType::asset) ||
            (account_types_ref[i] == AccountType::liability) ||
            (account_types_ref[i] == AccountType::equity)
        );
    }
}

}  // namespace test
}  // namespace dcm
