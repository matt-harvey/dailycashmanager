// Copyright (c) 2013, Matthew Harvey. All rights reserved.

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
