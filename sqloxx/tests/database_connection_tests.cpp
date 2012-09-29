#include "sqloxx/database_connection.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <unittest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::string;


using namespace sqloxx;

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
}  // End anonymous namespace



// Test DatabaseConnection default constructor, and open function. A one-off
// database file is created and then destroyed in this test.
TEST(test_default_constructor_and_open)
{
	string const filename("Testfile_00");	
	boost::filesystem::path const filepath(filename);
	if (boost::filesystem::exists(boost::filesystem::status(filepath)))
	{
		cerr << "File named \"" << filename << "\" already exists. "
		     << "Test aborted." << endl;
		std::abort();
	}
	DatabaseConnection dbc;
	CHECK(!dbc.is_valid());
	dbc.open(filename);
	CHECK(dbc.is_valid());
	CHECK(boost::filesystem::exists(boost::filesystem::status(filepath)));
	catch_check_ok(dbc);
	boost::filesystem::remove(filepath);
	assert (!boost::filesystem::exists(boost::filesystem::status(filepath)));
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
		assert (!dbc.is_valid());
	}

	// path to database file
	boost::filesystem::path filepath;
	DatabaseConnection dbc;
};


		


