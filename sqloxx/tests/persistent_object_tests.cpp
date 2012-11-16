#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/exception.hpp>
#include <jewel/optional.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include <typeinfo>
#include <unittest++/UnitTest++.h>
#include <stdexcept>

using boost::shared_ptr;
using jewel::UninitializedOptionalException;
using std::cerr;
using std::endl;
using std::numeric_limits;

namespace sqloxx
{
namespace tests
{

namespace filesystem = boost::filesystem;


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
	try  // WARNING temp
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
	catch (jewel::Exception& e)
	{
		cerr << typeid(e).name() << endl;
		cerr << e.what() << endl;
		throw;
	}
}

TEST_FIXTURE(DerivedPOFixture, test_derived_po_save_existing)
{
	DerivedPO dpo1(pdbc);
	dpo1.set_x(78);
	dpo1.set_y(4.5);
	CHECK_THROW(dpo1.save_existing(), LogicError);
	dpo1.save_new();
	DerivedPO dpo2(pdbc);
	dpo2.set_x(234);
	dpo2.set_y(29837.01);
	dpo2.save_new();
	CHECK_EQUAL(dpo2.id(), 2);
	DerivedPO dpo2b(pdbc, 2);
	CHECK_EQUAL(dpo2b.x(), 234);
	CHECK_EQUAL(dpo2b.y(), 29837.01);
	dpo2b.set_y(2.0);
	dpo2b.save_existing();
	DerivedPO dpo2c(pdbc, 2);
	CHECK_EQUAL(dpo2c.id(), 2);
	CHECK_EQUAL(dpo2c.x(), 234);
	CHECK_EQUAL(dpo2c.y(), 2.0);
	dpo2c.set_x(0);  // But don't call save_existing yet
	DerivedPO dpo2d(pdbc, 2);
	CHECK_EQUAL(dpo2d.x(), 234);
	CHECK_EQUAL(dpo2d.y(), 2.0);
	dpo2c.save_existing();  // Now the changed object is saved
	DerivedPO dpo2e(pdbc, 2);
	CHECK_EQUAL(dpo2e.x(), 0);
	CHECK_EQUAL(dpo2e.y(), 2.0);
	DerivedPO dpo2f(pdbc, 2);
	dpo2f.set_x(5000);
	dpo2f.save_existing();
	DerivedPO dpo2g(pdbc, 2);
	CHECK_EQUAL(dpo2g.x(), 5000);
	CHECK_EQUAL(dpo2g.y(), 2.0);
	DerivedPO dpo1b(pdbc, 1);
	dpo1b.save_existing();
	CHECK_EQUAL(dpo1b.x(), 78);
	CHECK_EQUAL(dpo1b.y(), 4.5);
}

TEST_FIXTURE(DerivedPOFixture, test_derived_po_save_new)
{
	DerivedPO dpo1(pdbc);
	dpo1.set_x(978);
	dpo1.set_y(-.238);
	dpo1.save_new();

	CHECK_THROW(dpo1.save_new(), LogicError);
	// Required to avoid incomplete transaction, which would be
	// expected, but which causes an annoying and pointless error
	// message to be printed when pdbc closed.
	pdbc->end_transaction();

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
	troublesome_statement.bind(":i", numeric_limits<int>::max());
	troublesome_statement.bind(":x", 30);
	troublesome_statement.bind(":y", 39.091);
	troublesome_statement.step_final();
	SharedSQLStatement check_troublesome
	(	*pdbc,
		"select derived_po_id from derived_pos where x = 30"
	);
	check_troublesome.step();
	CHECK_EQUAL(check_troublesome.extract<int>(0), numeric_limits<int>::max());
	check_troublesome.step_final();
	DerivedPO dpo3(pdbc);
	dpo3.set_x(100);
	dpo3.set_y(3.2);

	CHECK_THROW(dpo3.save_new(), TableSizeException);
	// Required to avoid incomplete transaction, which would be
	// expected, but which causes an annoying and pointless error
	// message to be printed when pdbc closed.
	pdbc->end_transaction();
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

	CHECK_THROW(dpo2.save_new(), LogicError);
	// Required to avoid incomplete transaction, which would be
	// expected, but which causes an annoying and pointless error
	// message to be printed when pdbc closed.
	pdbc->end_transaction();

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

TEST(test_derived_po_self_test)
{
	// Tests protected functions of PersistentObject
	CHECK_EQUAL(DerivedPO::self_test(), 0);
}


}  // namespace tests
}  // namespace sqloxx

