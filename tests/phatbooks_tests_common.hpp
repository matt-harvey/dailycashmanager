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

#ifndef GUARD_phatbooks_tests_common_hpp_7922174706087529
#define GUARD_phatbooks_tests_common_hpp_7922174706087529

#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

namespace phatbooks
{
namespace test
{


bool file_exists(boost::filesystem::path const& filepath);
void abort_if_exists(boost::filesystem::path const& dbc);
void setup_test_commodities(PhatbooksDatabaseConnection& dbc);
void setup_test_accounts(PhatbooksDatabaseConnection& dbc);

struct TestFixture
{
	// Setup
	TestFixture();

	// Teardown
	~TestFixture();

	// Database filepath
	boost::filesystem::path db_filepath;

	// The connection to the database
	PhatbooksDatabaseConnection* pdbc;
};








}  // namespace test
}  // namespace phatbooks


#endif  // GUARD_phatbooks_tests_common_hpp_7922174706087529
