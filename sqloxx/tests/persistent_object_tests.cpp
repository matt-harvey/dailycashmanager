#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/optional.hpp>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <unittest++/UnitTest++.h>

using boost::shared_ptr;
using jewel::UninitializedOptionalException;
using std::cerr;
using std::endl;

namespace sqloxx
{
namespace tests
{

namespace filesystem = boost::filesystem;

struct DerivedPOFixture
{
	// setup
	DerivedPOFixture():
		filepath("Testfile_02"),
		pdbc(new DatabaseConnection)
	{
		if (filesystem::exists(filesystem::status(filepath)))
		{
			cerr << "File named \"" << filepath.string()
			     << "\" already exists. Test aborted."
				 << endl;
			abort();
		}
		pdbc->open(filepath);
		assert (pdbc->is_valid());
		DerivedPO::setup_tables(*pdbc);
	}

	// teardown
	~DerivedPOFixture()
	{
		assert (pdbc->is_valid());
		filesystem::remove(filepath);
		assert (!file_exists(filepath));
	}

	// Database filepath
	filesystem::path filepath;

	// The connection to the database
	boost::shared_ptr<DatabaseConnection> pdbc;
};

TEST_FIXTURE(DerivedPOFixture, test_derived_po_constructor_A)
{
	DerivedPO dpo(pdbc);
	CHECK_THROW(dpo.id(), UninitializedOptionalException);
	CHECK_EQUAL(dpo.x(), 0);
}

}  // namespace tests
}  // namespace sqloxx

