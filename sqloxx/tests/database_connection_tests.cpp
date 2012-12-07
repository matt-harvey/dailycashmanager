#include "sqloxx_tests_common.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/next_auto_key.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/detail/sql_statement.hpp"
#include <unittest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

using boost::shared_ptr;
using std::cerr;
using std::endl;
using std::multiset;
using std::numeric_limits;
using std::ofstream;
using std::multiset;
using std::set;
using std::string;


namespace sqloxx
{
namespace tests
{


// Test DatabaseConnection default constructor, and open function. A one-off
// database file is created and then destroyed in this test.
TEST(test_default_constructor_and_open)
{
	// Define filepaths (without opening)
	boost::filesystem::path const filepath("T55555l2_009873");
	abort_if_exists(filepath);
	boost::filesystem::path const dummy_filepath("lasdy98989823");
	abort_if_exists(dummy_filepath);
	boost::filesystem::path const another_filepath("th887kk899872");
	abort_if_exists(another_filepath);
	boost::filesystem::path const empty_filepath("");
	abort_if_exists(empty_filepath);

	// Test opening new file
	DatabaseConnection dbc;
	CHECK(!dbc.is_valid());
	dbc.open(filepath);  // Note passing filename would also have worked
	CHECK(dbc.is_valid());
	CHECK(file_exists(filepath));

	// Test behaviour when calling open on an existing connection
	CHECK_THROW(dbc.open(filepath), MultipleConnectionException);
	CHECK(file_exists(filepath));

	// Test behaviour when calling open with some other filename corresponding
	// to a file that does not exist.
	CHECK_THROW(dbc.open(dummy_filepath), MultipleConnectionException);
	CHECK(!file_exists(dummy_filepath));
	CHECK(file_exists(filepath));

	// Test behaviour when calling open with some other filename corresponding
	// to a file that does exist, but is not connected to with any database
	// connection
	assert (!file_exists(another_filepath));
	ofstream ifs(another_filepath.string().c_str());
	assert (file_exists(another_filepath));
	CHECK_THROW
	(	dbc.open(another_filepath), MultipleConnectionException
	);
	boost::filesystem::remove(another_filepath);

	// Test opening with an empty string
	DatabaseConnection dbc2;
	CHECK(!dbc2.is_valid());
	CHECK_THROW(dbc2.open(empty_filepath), InvalidFilename);
	CHECK(!file_exists(empty_filepath));

	// Cleanup
	boost::filesystem::remove(filepath);
	assert (!boost::filesystem::exists(boost::filesystem::status(filepath)));
}	

TEST_FIXTURE(DatabaseConnectionFixture, test_is_valid)
{
	CHECK(dbc.is_valid());
	DatabaseConnection dbc2;
	CHECK(!dbc2.is_valid());
}



TEST(test_execute_sql_01)
{
	// Test on unopened DatabaseConnection
	string const command("create table test_table(column_A integer)");
	DatabaseConnection d;
	CHECK_THROW(d.execute_sql(command), InvalidConnection);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_execute_sql_02)
{
	dbc.execute_sql
	(	"create table test_table(column_A integer, column_B text not null)"
	);
	SharedSQLStatement statement_0
	(	dbc,
		"select column_A, column_B from test_table"
	);
	statement_0.step_final();
	dbc.execute_sql
	(	"insert into test_table(column_A, column_B) "
		"values(30, 'Hello')"
	);
	SharedSQLStatement statement_1
	(	dbc,
		"select column_A, column_B from test_table"
	);
	statement_1.step();
	int cell_0 = statement_1.extract<int>(0);
	CHECK_EQUAL(cell_0, 30);
	string cell_1 = statement_1.extract<string>(1);
	CHECK_EQUAL(cell_1, "Hello");
	statement_1.step_final();
	CHECK_THROW
	(	dbc.execute_sql("select mumbo jumbo"),
		SQLiteException
	);
	dbc.execute_sql("drop table test_table");
	CHECK_THROW
	(	dbc.execute_sql("select * from test_table"),
		SQLiteException
	);
}

TEST(test_next_auto_key_invalid_connection)
{
	DatabaseConnection db0;
	// Have to do this as CHECK_THROW gets confused by multiple template args
	bool ok = false;
	try
	{
		next_auto_key
		<	DatabaseConnection,
			int
		>	(db0, "dummy_table");
	}
	catch (InvalidConnection&)
	{
		ok = true;
	}
	CHECK(ok);
}
	

TEST_FIXTURE(DatabaseConnectionFixture, test_next_auto_key_normal)
{
	// Note CHECK_EQUAL and CHECK get confused by multiple template args
	bool ok =
		(next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	dbc.execute_sql
	(	"create table dummy_table(column_A text)"
	);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	dbc.execute_sql
	(	"create table test_table"
		"("
			"column_A integer not null unique, "
			"column_B integer primary key autoincrement, "
			"column_C text not null"
		")"
	);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "test_table") == 1);
	CHECK(ok);
	// This behaviour is strange but expected - see API docs.
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	dbc.execute_sql
	(	"insert into test_table(column_A, column_C) "
		"values(3, 'Hello')"
	);
	dbc.execute_sql
	(	"insert into test_table(column_A, column_C) "
		"values(4, 'Red')"
	);
	dbc.execute_sql
	(	"insert into test_table(column_A, column_C) "
		"values(10, 'Gold')"
	);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "test_table") == 4);
	CHECK(ok);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	
	// Test behaviour with gaps in numbering
	dbc.execute_sql("delete from test_table where column_B = 2");
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "test_table") == 4);
	
	// Key is not predicted to be reused once deleted
	dbc.execute_sql("delete from test_table where column_B = 3");
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "test_table") == 4);
	CHECK(ok);
	int const predicted_key =
		next_auto_key<DatabaseConnection, int>(dbc, "test_table");

	// Check key is not actually reused once deleted
	dbc.execute_sql
	(	"insert into test_table(column_A, column_C) "
		"values(110, 'Red')"
	);
	SharedSQLStatement statement2
	(	dbc,
		"select column_B from test_table where column_A = 110"
	);
	statement2.step();
	ok = (statement2.extract<int>(0) == predicted_key);
	CHECK(ok);
	statement2.step_final();

	// Test behaviour in protecting against overflow
	SharedSQLStatement statement
	(	dbc,
		"insert into test_table(column_A, column_B, column_C) "
		"values(:A, :B, :C)"
	);
	statement.bind(":A", 30);
	statement.bind(":B", numeric_limits<int>::max());
	statement.bind(":C", "Hello");
	statement.step_final();

	// Have to do this as CHECK_THROW gets confused by multiple template args
	try
	{
		next_auto_key<DatabaseConnection, int>(dbc, "test_table");
	}
	catch (TableSizeException&)
	{
		ok = true;
	}
	CHECK(ok);
	dbc.execute_sql("drop table dummy_table");
	dbc.execute_sql("drop table test_table");
}


TEST_FIXTURE(DatabaseConnectionFixture, test_setup_boolean_table)
{
	dbc.setup_boolean_table();
	SharedSQLStatement statement(dbc, "select representation from booleans");
	multiset<int> result;
	while (statement.step())
	{
		result.insert(statement.extract<int>(0));
	}
	CHECK(result.find(0) != result.end());
	CHECK(result.find(1) != result.end());
	CHECK_EQUAL(result.size(), 2);
	CHECK_THROW(dbc.setup_boolean_table(), SQLiteException);
	dbc.execute_sql("drop table booleans");
	DatabaseConnection invaliddb;
	CHECK_THROW(invaliddb.setup_boolean_table(), InvalidConnection);
}


TEST_FIXTURE(DatabaseConnectionFixture, test_transaction_nesting_exception_01)
{
	CHECK_THROW(dbc.end_transaction(), TransactionNestingException);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_transaction_nesting_exception_02)
{
	dbc.begin_transaction();
	dbc.execute_sql("create table dummy(col_A text, col_B text)");
	dbc.execute_sql
	(	"insert into dummy(col_A, col_B) values('Hello', 'Goodbye')"
	);
	dbc.execute_sql
	(	"insert into dummy(col_A, col_B) values('Yeah!', 'What!')"
	);
	dbc.begin_transaction();
	SharedSQLStatement statement
	(	dbc,
		"select col_A from dummy where col_B = 'Goodbye'"
	);
	statement.step();
	statement.step_final();
	dbc.end_transaction();
	dbc.end_transaction();
	CHECK_THROW(dbc.end_transaction(), TransactionNestingException);
}

TEST_FIXTURE
(	DatabaseConnectionFixture,
	test_transaction_begin_and_end_response_to_invalid_connection
)
{
	DatabaseConnection invaliddbc;
	CHECK_THROW(invaliddbc.begin_transaction(), InvalidConnection);
	DatabaseConnection invaliddbc2;
	CHECK_THROW(invaliddbc2.end_transaction(), TransactionNestingException);
	DatabaseConnection invaliddbc3;
	try
	{
		invaliddbc3.begin_transaction();
	}
	catch (InvalidConnection&)
	{
	}
	// Transaction begin should not have gone ahead, so...
	CHECK_THROW(invaliddbc3.end_transaction(), TransactionNestingException);
}

/*
 * For test of whether DatabaseConnection::begin_transaction and
 * DatabaseConnection::end_transaction actually do enable access to
 * the atomicity of their underlying SQL commands, see
 * atomicity_test.hpp, test.cpp and test.tcl.
 */

TEST_FIXTURE(DatabaseConnectionFixture, test_sqlite_rollback)
{
	// This is not actually aimed at testing anything in sqloxx, but is more
	// aimed at verifying that certain SQLite SQL commands do what we expect.
	dbc.execute_sql("create table dummy(col_A integer)");
	dbc.execute_sql("begin transaction");
	dbc.execute_sql("insert into dummy(col_A) values(3)");
	dbc.execute_sql("savepoint sp");
	dbc.execute_sql("insert into dummy(col_A) values(4)");
	dbc.execute_sql("rollback to savepoint sp");
	dbc.execute_sql("release sp");
	dbc.execute_sql("end transaction");
	SharedSQLStatement s1(dbc, "select col_A from dummy where col_A = 3");
	s1.step();
	CHECK_EQUAL(s1.extract<int>(0), 3);
	s1.step_final();
	SharedSQLStatement s2(dbc, "select col_A from dummy where col_A = 4");
	CHECK_EQUAL(s2.step(), false);
	SharedSQLStatement s3(dbc, "select * from dummy");
	s3.step();
	s3.step_final();  // As only one record.
}

TEST_FIXTURE(DatabaseConnectionFixture, test_cancel_transaction)
{
	dbc.execute_sql("create table dummy(col_A)");
	dbc.begin_transaction();
	dbc.execute_sql("insert into dummy(col_A) values(3)");
	dbc.begin_transaction();
	dbc.execute_sql("insert into dummy(col_A) values(4)");
	dbc.cancel_transaction();
	dbc.end_transaction();
	SharedSQLStatement s1(dbc, "select * from dummy");
	CHECK_EQUAL(s1.step(), true);
	CHECK_EQUAL(s1.extract<int>(0), 3);
	CHECK_EQUAL(s1.step(), false);

	dbc.begin_transaction();
	dbc.execute_sql("insert into dummy(col_A) values(100)");
	dbc.cancel_transaction();
	CHECK_THROW(dbc.end_transaction(), TransactionNestingException);
	SharedSQLStatement s2(dbc, "select * from dummy where col_A = 100");
	CHECK_EQUAL(s2.step(), false);

	dbc.begin_transaction();
	dbc.execute_sql("insert into dummy(col_A) values(200)");
	dbc.begin_transaction();
	dbc.end_transaction();
	dbc.cancel_transaction();
	CHECK_THROW(dbc.cancel_transaction(), TransactionNestingException);
	SharedSQLStatement s3(dbc, "select * from dummy where col_A = 200");
	CHECK_EQUAL(s3.step(), false);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_provide_sql_statement)
{
	dbc.execute_sql("create table dummy(col_A int, col_B int)");
	shared_ptr<detail::SQLStatementImpl> statement_pointer =
		dbc.provide_sql_statement
		(	"insert into dummy(col_A, col_B) values(300, 10)"
		);
	SharedSQLStatement selector_01
	(	dbc,
		"select col_A from dummy where col_B = 10"
	);
	bool const first_go = selector_01.step();
	CHECK_EQUAL(first_go, false);
	bool const inserting = statement_pointer->step();
	CHECK_EQUAL(inserting, false);
	selector_01.reset();
	bool const second_go = selector_01.step();
	CHECK_EQUAL(second_go, true);
	CHECK_EQUAL(selector_01.extract<int>(0), 300);
	selector_01.step_final();
}



}  // namespace tests
}  // namespace sqloxx
