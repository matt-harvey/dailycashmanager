#include "sqloxx_tests_common.hpp"
#include "sqloxx/database_connection.hpp"
#include <boost/filesystem.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>

using std::abort;
using std::cerr;
using std::endl;


namespace sqloxx
{
namespace tests
{


bool file_exists(boost::filesystem::path const& filepath)
{
	return boost::filesystem::exists
	(	boost::filesystem::status(filepath)
	);
}


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


DatabaseConnectionFixture::DatabaseConnectionFixture():
	filepath("Testfile_01")
{
	if (boost::filesystem::exists(boost::filesystem::status(filepath)))
	{
		cerr << "File named \"" << filepath.string()
		     << "\" already exists. Test aborted."
			 << endl;
		abort();
	}
	dbc.open(filepath);
	assert (dbc.is_valid());
}


DatabaseConnectionFixture::~DatabaseConnectionFixture()
{
	assert (dbc.is_valid());
	boost::filesystem::remove(filepath);
	assert (!file_exists(filepath));
}


}  // namespace sqloxx
}  // namespace tests
