#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/optional.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <unittest++/UnitTest++.h>

using boost::shared_ptr;
using jewel::UninitializedOptionalException;
using std::cerr;
using std::endl;
using std::logic_error;
using std::numeric_limits;

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

TEST_FIXTURE(DerivedPOFixture, test_derived_po_save_existing)
{
	// WARNING Unimplemented test.
}

TEST_FIXTURE(DerivedPOFixture, test_derived_po_save_new)
{
	DerivedPO dpo1(pdbc);
	dpo1.set_x(978);
	dpo1.set_y(-.238);
	dpo1.save_new();
	// WARNING The next line causes "transactions remain
	// incomplete on closing of DatabaseConnection". Does this matter?
	CHECK_THROW(dpo1.save_new(), logic_error);
	DerivedPO dpo2(pdbc);
	dpo2.set_x(20);
	dpo2.set_y(0.00030009);
	dpo2.save_new();
	CHECK_EQUAL(dpo1.id(), 1);
	CHECK_EQUAL(dpo2.id(), 2);
	SharedSQLStatement troublesome_statement
	(	*pdbc,
		"insert into derived_pos(derived_po_id, x, y) values"
		"(:i, :x, :y)"
	);
	typedef PersistentObject::Id Id;
	troublesome_statement.bind(":i", numeric_limits<Id>::max());
	troublesome_statement.bind(":x", 30);
	troublesome_statement.bind(":y", 39.091);
	troublesome_statement.step_final();
	SharedSQLStatement check_troublesome
	(	*pdbc,
		"select derived_po_id from derived_pos where x = 30"
	);
	check_troublesome.step();
	CHECK_EQUAL(check_troublesome.extract<Id>(0), numeric_limits<Id>::max());
	check_troublesome.step_final();
	DerivedPO dpo3(pdbc);
	dpo3.set_x(100);
	dpo3.set_y(3.2);
	CHECK_THROW(dpo3.save_new(), TableSizeException);
}

TEST_FIXTURE(DerivedPOFixture, test_derived_po_id_getter)
{
	DerivedPO dpo1(pdbc);
	CHECK_THROW(dpo1.id(), UninitializedOptionalException);
	dpo1.save_new();
	CHECK_EQUAL(dpo1.id(), 1);
	DerivedPO dpo2(pdbc);
	CHECK_THROW(dpo2.id(), UninitializedOptionalException);
	CHECK_THROW(dpo2.id(), UninitializedOptionalException);
	dpo2.save_new();
	CHECK_EQUAL(dpo2.id(), 2);
	// WARNING The next line causes transaction to be incomplete on closing
	// of DatabaseConnection. Does this matter?
	CHECK_THROW(dpo2.save_new(), logic_error);
	CHECK_EQUAL(dpo2.id(), 2);
}

TEST_FIXTURE(DerivedPOFixture, test_load_indirectly)
{
	// load is protected method but we here we test it indirectly
	// via getting functions we know call it
	DerivedPO dpo1(pdbc);
	int const a = 2097601234;
	double const b = 72973.2987300;
	dpo1.set_x(a);
	dpo1.set_y(b);
	assert (dpo1.x() == a);
	assert (dpo1.y() == b);
	dpo1.save_new();

	DerivedPO dpo2(pdbc, 1);
	CHECK_EQUAL(dpo2.id(), 1);
	CHECK_EQUAL(dpo2.x(), a);  // load called here
	CHECK_EQUAL(dpo2.y(), b);  // and here
}

// todo Write remaining tests. Should test protected methods of
// PersistentObject by writing special test methods in DerivedPO itself
// that call those methods.

}  // namespace tests
}  // namespace sqloxx

