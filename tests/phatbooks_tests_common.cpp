#include "phatbooks_tests_common.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

using std::abort;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

namespace filesystem = boost::filesystem;

namespace phatbooks
{
namespace test
{


bool file_exists(filesystem::path const& filepath)
{
	return filesystem::exists
	(	filesystem::status(filepath)
	);
}


void abort_if_exists(filesystem::path const& filepath)
{
	if (file_exists(filepath))
	{
		cerr << "File named \"" << filepath.string() << "\" already "
			 << "exists. Test aborted." << endl;
		abort();
	}
	return;
}

TestFixture::TestFixture():
	db_filepath("Testfile_8287293")
{
	abort_if_exists(db_filepath);
	dbc.open(db_filepath);
	assert (dbc.is_valid());
}

TestFixture::~TestFixture()
{
	assert (dbc.is_valid());
	filesystem::remove(db_filepath);
	assert (!file_exists(db_filepath));
}








}  // namespace test
}  // namespace phatbooks
