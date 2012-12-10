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

TEST_FIXTURE(DerivedPOFixture, test_derived_po_save_2)
{
	Handle<DerivedPO> dpo1(get_handle<DerivedPO>(*pdbc));
	dpo1->set_x(978);
	dpo1->set_y(-.238);
	dpo1->save();

	Handle<DerivedPO> dpo2(get_handle<DerivedPO>(*pdbc));
	dpo2->set_x(20);
	dpo2->set_y(0.00030009);
	dpo2->save();
	CHECK_EQUAL(dpo1->id(), 1);
	CHECK_EQUAL(dpo2->id(), 2);

	// Test TransactionNestingException
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
	CHECK_EQUAL
	(	check_troublesome.extract<int>(0),
		numeric_limits<int>::max()
	);
	check_troublesome.step_final();

	Handle<DerivedPO> dpo3(get_handle<DerivedPO>(*pdbc));
	dpo3->set_x(100);
	dpo3->set_y(3.2);

	CHECK_THROW(dpo3->save(), TableSizeException);
	
	// But these are still OK
	dpo1->save();
	dpo1->save();
	dpo1->save();
}


TEST_FIXTURE(DerivedPOFixture, test_derived_po_save_and_transactions)
{
	// Test interaction of save() with DatabaseTransaction

	Handle<DerivedPO> dpo1(get_handle<DerivedPO>(*pdbc));
	dpo1->set_x(4000);
	dpo1->set_y(0.13);
	dpo1->save();

	DatabaseTransaction transaction1(*pdbc);

	Handle<DerivedPO> dpo2(get_handle<DerivedPO>(*pdbc));
	dpo2->set_x(-17);
	dpo2->set_y(64.29382);
	dpo2->save();
	
	Handle<DerivedPO> dpo2b(get_handle<DerivedPO>(*pdbc, 2));
	CHECK_EQUAL(dpo2b->x(), -17);
	CHECK_EQUAL(dpo2b->y(), 64.29382);
	dpo2b->save();

	CHECK_EQUAL(dpo1->id(), 1);
	CHECK_EQUAL(dpo2->id(), 2);
	CHECK_EQUAL(dpo2b->id(), 2);

	Handle<DerivedPO> dpo3(get_handle<DerivedPO>(*pdbc));
	dpo3->set_x(7834);
	dpo3->set_y(521.520);
	CHECK(!dpo3->has_id());
	dpo3->save();
	CHECK_EQUAL(dpo3->id(), 3);

	Handle<DerivedPO> dpo4(get_handle<DerivedPO>(*pdbc));
	dpo4->set_y(1324.6);
	dpo4->set_x(321);
	dpo4->save();
	CHECK_EQUAL(dpo4->id(), 4);

	transaction1.cancel();

	SQLStatement statement(*pdbc, "select * from derived_pos");
	int rows = 0;
	while (statement.step()) ++rows;
	CHECK_EQUAL(rows, 1);

	// The cache is not aware in itself that the save was cancelled...
	Handle<DerivedPO> dpo2c(unchecked_get_handle<DerivedPO>(*pdbc, 2));
	CHECK_EQUAL(dpo2c->id(), 2);
	CHECK_EQUAL(dpo2c->x(), -17);
	CHECK_EQUAL(dpo2c->y(), 64.29382);

	// ... That's why we should not use unchecked_get_handle unless
	// we're sure we've got a valid id. The "normal" get_handle
	// throws here.
	CHECK_THROW
	(	Handle<DerivedPO> dpo2c_checked(get_handle<DerivedPO>(*pdbc, 2)),
		BadIdentifier
	);

	// At least this will save over the top of the old
	// one...
	Handle<DerivedPO> dpo5(get_handle<DerivedPO>(*pdbc));
	dpo5->set_x(12);
	dpo5->set_y(19);
	dpo5->save();

	CHECK_EQUAL(dpo5->id(), 2);
	CHECK_EQUAL(dpo5->x(), 12);
	CHECK_EQUAL(dpo5->y(), 19);
	
	// The objects still have attributes that we can retrieve
	CHECK_EQUAL(dpo2b->x(), -17);
	CHECK_EQUAL(dpo2->y(), 64.29382);

	Handle<DerivedPO> dpo2d(get_handle<DerivedPO>(*pdbc, 2));
	CHECK_EQUAL(dpo2d->x(), 12);
	CHECK_EQUAL(dpo2d->y(), 19);

	CHECK_THROW
	(	Handle<DerivedPO> dpo7(get_handle<DerivedPO>(*pdbc, 7)),
		BadIdentifier
	);
	
	CHECK_EQUAL(dpo4->id(), 4);
	CHECK_EQUAL(dpo4->x(), 321);
	CHECK_EQUAL(dpo4->y(), 1324.6);
	CHECK_THROW
	(	Handle<DerivedPO> dpo4b(get_handle<DerivedPO>(*pdbc, 4)),
		BadIdentifier
	);

	// We can remove it dpo4 the cache like this - even though
	// it has already been removed from the database
	dpo4->remove();
	CHECK_THROW(dpo4->id(), UninitializedOptionalException);
	// But it still exists in memory with its attributes. That's OK.
	CHECK_EQUAL(dpo4->x(), 321);
	CHECK_EQUAL(dpo4->y(), 1324.6);
}
// WARNING Reworking of tests is up to here.

TEST_FIXTURE(DerivedPOFixture, test_derived_po_id_getter)
{
	Handle<DerivedPO> dpo1(get_handle<DerivedPO>(*pdbc));
	CHECK_THROW(dpo1->id(), UninitializedOptionalException);
	dpo1->save();
	CHECK_EQUAL(dpo1->id(), 1);
	Handle<DerivedPO> dpo2(get_handle<DerivedPO>(*pdbc));
	CHECK_THROW(dpo2->id(), UninitializedOptionalException);
	CHECK_THROW(dpo2->id(), UninitializedOptionalException);
	dpo2->save();
	CHECK_EQUAL(dpo2->id(), 2);
	dpo2->save();
	CHECK_EQUAL(dpo2->id(), 2);
}

TEST_FIXTURE(DerivedPOFixture, test_load_indirectly)
{
	// load is protected method but we here we test it indirectly
	// via getting functions we know call it
	Handle<DerivedPO> dpo1(get_handle<DerivedPO>(*pdbc));
	int const a = 2097601234;
	double const b = 72973.2987300;
	dpo1->set_x(a);
	dpo1->set_y(b);
	assert (dpo1->x() == a);
	assert (dpo1->y() == b);
	dpo1->save();

	Handle<DerivedPO> dpo2(get_handle<DerivedPO>(*pdbc, 1));
	CHECK_EQUAL(dpo2->id(), 1);
	CHECK_EQUAL(dpo2->x(), a);  // load called here
	CHECK_EQUAL(dpo2->y(), b);  // and here
}

TEST(test_derived_po_self_test)
{
	// Tests certain protected functions of PersistentObject
	CHECK_EQUAL(DerivedPO::self_test(), 0);
}

}  // namespace tests
}  // namespace sqloxx

