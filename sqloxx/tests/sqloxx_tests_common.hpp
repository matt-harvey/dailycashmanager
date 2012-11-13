#ifndef GUARD_sqloxx_tests_common_hpp
#define GUARD_sqloxx_tests_common_hpp

// Hide from Doxygen
/// @cond


/**
 * @file sqloxx_tests_common.hpp
 *
 * Contains code for common use by Sqloxx unit tests (rather
 * than specific to any particular class).
 */

#include "sqloxx/database_connection.hpp"
#include <unittest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <iostream>


namespace sqloxx
{
namespace tests
{


bool file_exists(boost::filesystem::path const& filepath);

void catch_check_ok(DatabaseConnection& dbc);

void abort_if_exists(boost::filesystem::path const& filepath);

// To compare speed of SQLStatement with SharedSQLStatement, to
// evaluate effectiveness of caching in latter.
void do_speed_test();

// Fixture that creates a DatabaseConnection and database file for
// reuse in tests.
struct DatabaseConnectionFixture
{
	// setup
	DatabaseConnectionFixture();

	// teardown
	~DatabaseConnectionFixture();

	// Database filepath
	boost::filesystem::path filepath;

	// The connection to the database
	DatabaseConnection dbc;
};

struct DerivedPOFixture
{
	// setup
	DerivedPOFixture();

	// teardown
	~DerivedPOFixture();

	// Database filepath
	boost::filesystem::path filepath;

	// The connection to the database
	boost::shared_ptr<DatabaseConnection> pdbc;
};




}  // namespace tests
}  // namespace sqloxx

/// @endcond
// End hiding from Doxygen

#endif  // GUARD_sqloxx_tests_common_hpp
