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

TEST_FIXTURE(DerivedPOFixture, test_derived_po_constructor_one_param)
{
	DerivedPO dpo(pdbc);
	CHECK_THROW(dpo.id(), UninitializedOptionalException);
	CHECK_EQUAL(dpo.x(), 0);
	dpo.set_y(3.3);
	CHECK_EQUAL(dpo.y(), 3.3);
}

TEST_FIXTURE(DerivedPOFixture, test_derived_po_constructor_two_params)
{
	DerivedPO dpo(pdbc);
	dpo.set_x(10);
	dpo.set_y(3.23);
	dpo.save_new();
	CHECK_EQUAL(dpo.id(), 1);
	CHECK_EQUAL(dpo.x(), 10);
	CHECK_EQUAL(dpo.y(), 3.23);
	DerivedPO e(pdbc, 1);
	CHECK_EQUAL(e.id(), dpo.id());
	CHECK_EQUAL(e.id(), 1);
	CHECK_EQUAL(e.x(), 10);
	CHECK_EQUAL(e.y(), 3.23);
}

}  // namespace tests
}  // namespace sqloxx

