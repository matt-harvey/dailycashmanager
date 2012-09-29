#include "sqloxx/database_connection.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <unittest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::ofstream;
using std::string;


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
		assert (!dbc.is_valid());
	}

	// path to database file
	boost::filesystem::path filepath;
	DatabaseConnection dbc;
};


		

}  // namespace sqloxx
