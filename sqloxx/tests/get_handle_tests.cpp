#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/handle.hpp"
#include <unittest++/UnitTest++.h>

namespace sqloxx
{
namespace tests
{

TEST_FIXTURE(DerivedPOFixture, test_get_handle)
{
	Handle<DerivedPO> dpo1(get_handle<DerivedPO>(*pdbc));
	Handle<DerivedPO> dpo2(dpo1);
	dpo1->set_x(30010);
	dpo2->set_y(200.098);
	CHECK_EQUAL(dpo2->x(), 30010);
	CHECK_EQUAL(dpo1->y(), 200.098);
	CHECK_EQUAL(dpo1->x(), dpo2->x());
	CHECK_EQUAL(dpo1->y(), dpo2->y());
	dpo2->save();
	CHECK_EQUAL(dpo1->id(), dpo2->id());
	CHECK_EQUAL(dpo1->id(), 1);
	dpo1->save();
	CHECK_EQUAL(dpo1->id(), 1);
	CHECK_EQUAL(dpo2->id(), 1);
	Handle<DerivedPO> dpo3(get_handle<DerivedPO>(*pdbc));
	dpo3->set_x(10);
	dpo3->set_y(-1987.09);
	dpo3->save();
	CHECK(dpo1->id() != dpo3->id());
	CHECK_EQUAL(get_handle<DerivedPO>(*pdbc, 2)->y(), dpo3->y());
	CHECK_EQUAL(dpo1->id(), get_handle<DerivedPO>(*pdbc, 1)->id());
}


}  // namespace tests
}  // namespace sqloxx
