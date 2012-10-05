#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/tests/sqloxx_tests_common.hpp"

#include <unittest++/UnitTest++.h>

namespace sqloxx
{
namespace tests
{


TEST_FIXTURE(DatabaseConnectionFixture, test_shared_sql_statement_constructor)
{
	// Check behaviour with SQL syntax error
	CHECK_THROW
	(	SharedSQLStatement unsyntactic(dbc, "unsyntactical gobbledigook"),
		SQLiteException
	);
	
	// Check behaviour with invalid database connection
	DatabaseConnection temp_dbc;
	CHECK_THROW
	(	SharedSQLStatement unconnected_to_file
		(	temp_dbc,
			"create table dummy(Col_A text);"
		),
		InvalidConnection
	);
}

// WARNING INCOMPLETE TESTS




}  // namespace sqloxx
}  // namespace tests
