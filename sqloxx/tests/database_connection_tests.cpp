#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <unittest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <cassert>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::numeric_limits;
using std::ofstream;
using std::string;
using std::vector;


namespace sqloxx
{

// Anonymous namespace
namespace
{
	void catch_check_ok(DatabaseConnection& dbc)
	{
		try
		{
			dbc.check_ok();
		}
		catch (SQLiteException& e)
		{
			cerr << "Exception caught by DatabaseConnection::check_ok()."
			     << endl;
			cerr << "Error message: " << e.what() << endl;
			cerr << "Failing test." << endl;
			CHECK(false);
		}
		return;
	}

	bool file_exists(boost::filesystem::path const& filepath)
	{
		return boost::filesystem::exists
		(	boost::filesystem::status(filepath)
		);
	}

	void abort_if_exists(boost::filesystem::path const& filepath)
	{
		if (file_exists(filepath))
		{
			cerr << "File named \"" << filepath.string() << "\" already "
			     << "exists. Test aborted." << endl;
			std::abort();
		}
		return;
	}
			    
}  // End anonymous namespace



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
	catch_check_ok(dbc);

	// Test behaviour when calling open on an existing connection
	CHECK_THROW(dbc.open(filepath), MultipleConnectionException);
	CHECK(file_exists(filepath));
	catch_check_ok(dbc);

	// Test behaviour when calling open with some other filename corresponding
	// to a file that does not exist.
	CHECK_THROW(dbc.open(dummy_filepath), MultipleConnectionException);
	CHECK(!file_exists(dummy_filepath));
	CHECK(file_exists(filepath));
	catch_check_ok(dbc);

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
	catch_check_ok(dbc);

	// Test opening with an empty string
	DatabaseConnection dbc2;
	CHECK(!dbc2.is_valid());
	CHECK_THROW(dbc2.open(empty_filepath), InvalidFilename);
	CHECK(!file_exists(empty_filepath));

	// Cleanup
	boost::filesystem::remove(filepath);
	assert (!boost::filesystem::exists(boost::filesystem::status(filepath)));
	assert (!file_exists(filepath));
}	



// Fixture that creates a DatabaseConnection and database file for
// reuse in tests.
struct DatabaseConnectionFixture
{
	// setup
	DatabaseConnectionFixture():
		filepath("Testfile_01")
	{
		if (boost::filesystem::exists(boost::filesystem::status(filepath)))
		{
			cerr << "File named \"" << filepath.string()
			     << "\" already exists. Test aborted."
				 << endl;
			std::abort();
		}
		dbc.open(filepath);
		assert (dbc.is_valid());
	}

	// teardown
	~DatabaseConnectionFixture()
	{
		assert (dbc.is_valid());
		boost::filesystem::remove(filepath);
		assert
		(	!boost::filesystem::exists(boost::filesystem::status(filepath))
		);
		// assert (!dbc.is_valid());
	}

	// path to database file
	boost::filesystem::path filepath;
	DatabaseConnection dbc;
};


TEST_FIXTURE(DatabaseConnectionFixture, test_is_valid)
{
	CHECK(dbc.is_valid());
	DatabaseConnection dbc2;
	CHECK(!dbc2.is_valid());
}


TEST_FIXTURE(DatabaseConnectionFixture, test_execute_sql)
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

TEST_FIXTURE(DatabaseConnectionFixture, test_check_ok)
{
	dbc.check_ok();  // Should not throw
	try
	{
		dbc.execute_sql
		(	"create mumbo jumbo"
		);
	}
	catch (SQLiteException&)
	{
		// Do nothing
	}
	CHECK_THROW(dbc.check_ok(), SQLiteException);
	DatabaseConnection temp;
	CHECK_THROW(temp.check_ok(), SQLiteException);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_primary_key)
{
	dbc.execute_sql
	(	"create table dummy"
		"("
			"column_A integer not null, "
			"column_B text, "
			"column_C text not null, "
			"column_D float not null unique, "
			"primary key(column_A, column_C)  "
		");"
	);
	vector<string> vec = dbc.primary_key("dummy");
	CHECK_EQUAL(vec.size(), 2);
	CHECK_EQUAL(vec[0], "column_A");
	CHECK_EQUAL(vec[1], "column_C");
	dbc.execute_sql
	(	"create table dummyB"
		"("
			"column_BA text, "
			"column_BB integer primary key autoincrement"
		");"
	);
	vector<string> vecB = dbc.primary_key("dummyB");
	CHECK_EQUAL(vecB.size(), 1);
	CHECK_EQUAL(vecB[0], "column_BB");
	dbc.execute_sql
	(	"create table dummyC"
		"("
			"column_CA integer not null unique, "
			"column_CB float not null unique, "
			"column_CC text unique"
		");"
	);
	vector<string> vecC = dbc.primary_key("dummyC");
	CHECK_EQUAL(vecC.size(), 0);
	dbc.execute_sql
	(	"drop table dummy; drop table dummyB; drop table dummyC;"
	);
	dbc.check_ok();
}

TEST_FIXTURE(DatabaseConnectionFixture, test_next_auto_key)
{
	dbc.execute_sql
	(	"create table dummy_table(column_A text)"
	);
	CHECK_THROW(dbc.next_auto_key<int>("dummy_table"), SQLiteException);
	CHECK_THROW(dbc.next_auto_key<int>("test_table"), SQLiteException);
	dbc.execute_sql
	(	"create table test_table"
		"("
			"column_A integer not null unique, "
			"column_B integer primary key autoincrement, "
			"column_C text not null"
		")"
	);
	CHECK_EQUAL(dbc.next_auto_key<int>("test_table"), 1);
	// This behaviour is strange but expected - see API docs.
	CHECK_EQUAL(dbc.next_auto_key<int>("dummy_table"), 1);
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
	CHECK_EQUAL(dbc.next_auto_key<int>("test_table"), 4);
	CHECK_EQUAL(dbc.next_auto_key<int>("dummy_table"), 1);
	
	// Test behaviour with gaps in numbering
	dbc.execute_sql("delete from test_table where column_B = 2");
	CHECK_EQUAL(dbc.next_auto_key<int>("test_table"), 4);
	
	// Key is not predicted to be reused once deleted
	dbc.execute_sql("delete from test_table where column_B = 3");
	CHECK_EQUAL(dbc.next_auto_key<int>("test_table"), 4);
	int const predicted_key = dbc.next_auto_key<int>("test_table");

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
	CHECK_EQUAL(statement2.extract<int>(0), predicted_key);
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
	CHECK_THROW
	(	dbc.next_auto_key<int>("test_table"), TableSizeException
	);
	dbc.execute_sql("drop table dummy_table");
	dbc.execute_sql("drop table test_table");
}




		

}  // namespace sqloxx
