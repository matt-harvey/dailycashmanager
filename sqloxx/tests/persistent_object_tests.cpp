#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/handle.hpp"
#include "sqloxx/sql_statement.hpp"
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
	Handle<DerivedPO> dpo(get_handle<DerivedPO>(*pdbc));
	CHECK_THROW(dpo->id(), UninitializedOptionalException);
	CHECK_EQUAL(dpo->x(), 0);
	dpo->set_y(3.3);
	CHECK_EQUAL(dpo->y(), 3.3);
}


TEST_FIXTURE(DerivedPOFixture, test_derived_po_constructor_two_params)
{
	Handle<DerivedPO> dpo(get_handle<DerivedPO>(*pdbc));
	dpo->set_x(10);
	dpo->set_y(3.23);
	dpo->save();
	CHECK_EQUAL(dpo->id(), 1);
	CHECK_EQUAL(dpo->x(), 10);
	CHECK_EQUAL(dpo->y(), 3.23);
	Handle<DerivedPO> e(get_handle<DerivedPO>(*pdbc, 1));
	CHECK_EQUAL(e->id(), dpo->id());
	CHECK_EQUAL(e->id(), 1);
	CHECK_EQUAL(e->x(), 10);
	CHECK_EQUAL(e->y(), 3.23);
}

TEST_FIXTURE(DerivedPOFixture, test_derived_po_save_1)
{
	Handle<DerivedPO> dpo1(get_handle<DerivedPO>(*pdbc));
	dpo1->set_x(78);
	dpo1->set_y(4.5);
	dpo1->save();
	Handle<DerivedPO> dpo2(get_handle<DerivedPO>(*pdbc));
	dpo2->set_x(234);
	dpo2->set_y(29837.01);
	dpo2->save();
	CHECK_EQUAL(dpo2->id(), 2);
	Handle<DerivedPO> dpo2b(get_handle<DerivedPO>(*pdbc, 2));
	CHECK_EQUAL(dpo2b->x(), 234);
	CHECK_EQUAL(dpo2b->y(), 29837.01);
	dpo2b->set_y(2.0);
	dpo2b->save();
	Handle<DerivedPO> dpo2c(get_handle<DerivedPO>(*pdbc, 2));
	CHECK_EQUAL(dpo2c->id(), 2);
	CHECK_EQUAL(dpo2c->x(), 234);
	CHECK_EQUAL(dpo2c->y(), 2.0);
	dpo2c->set_x(-10);  // But don't call save yet
	Handle<DerivedPO> dpo2d(get_handle<DerivedPO>(*pdbc, 2));
	CHECK_EQUAL(dpo2d->x(), -10); // Reflected before save, due to IdentityMap
	CHECK_EQUAL(dpo2d->y(), 2.0);
	dpo2c->save();  // Now the changed object is saved
	Handle<DerivedPO> dpo2e(get_handle<DerivedPO>(*pdbc, 2));
	CHECK_EQUAL(dpo2e->x(), -10);  // Still reflected after save.
	CHECK_EQUAL(dpo2e->y(), 2.0);
	Handle<DerivedPO> dpo1b(get_handle<DerivedPO>(*pdbc, 1));
	dpo1b->save();
	CHECK_EQUAL(dpo1b->x(), 78);
	CHECK_EQUAL(dpo1b->y(), 4.5);
	dpo1b->set_x(1000);
	// All handles to this object reflect the change
	CHECK_EQUAL(dpo1->x(), 1000);
}

// todo Tests need updating below here.
TEST_FIXTURE(DerivedPOFixture, test_derived_po_save_2)
{
	DerivedPO dpo1(pdbc->derived_po_map());
	dpo1.set_x(978);
	dpo1.set_y(-.238);
	dpo1.save();

	DerivedPO dpo2(pdbc->derived_po_map());
	dpo2.set_x(20);
	dpo2.set_y(0.00030009);
	dpo2.save();
	CHECK_EQUAL(dpo1.id(), 1);
	CHECK_EQUAL(dpo2.id(), 2);
	SQLStatement troublesome_statement
	(	*pdbc,
		"insert into derived_pos(derived_po_id, x, y) values"
		"(:i, :x, :y)"
	);
	troublesome_statement.bind(":i", numeric_limits<int>::max());
	troublesome_statement.bind(":x", 30);
	troublesome_statement.bind(":y", 39.091);
	troublesome_statement.step_final();
	SQLStatement check_troublesome
	(	*pdbc,
		"select derived_po_id from derived_pos where x = 30"
	);
	check_troublesome.step();
	CHECK_EQUAL(check_troublesome.extract<int>(0), numeric_limits<int>::max());
	check_troublesome.step_final();
	DerivedPO dpo3(pdbc->derived_po_map());
	dpo3.set_x(100);
	dpo3.set_y(3.2);

	CHECK_THROW(dpo3.save(), TableSizeException);
}

TEST_FIXTURE(DerivedPOFixture, test_derived_po_id_getter)
{
	DerivedPO dpo1(pdbc->derived_po_map());
	CHECK_THROW(dpo1.id(), UninitializedOptionalException);
	dpo1.save();
	CHECK_EQUAL(dpo1.id(), 1);
	DerivedPO dpo2(pdbc->derived_po_map());
	CHECK_THROW(dpo2.id(), UninitializedOptionalException);
	CHECK_THROW(dpo2.id(), UninitializedOptionalException);
	dpo2.save();
	CHECK_EQUAL(dpo2.id(), 2);
	dpo2.save();
	CHECK_EQUAL(dpo2.id(), 2);
}

TEST_FIXTURE(DerivedPOFixture, test_load_indirectly)
{
	// load is protected method but we here we test it indirectly
	// via getting functions we know call it
	DerivedPO dpo1(pdbc->derived_po_map());
	int const a = 2097601234;
	double const b = 72973.2987300;
	dpo1.set_x(a);
	dpo1.set_y(b);
	assert (dpo1.x() == a);
	assert (dpo1.y() == b);
	dpo1.save();

	DerivedPO dpo2(pdbc->derived_po_map(), 1);
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

