#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/identity_map.hpp"
#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include <unittest++/UnitTest++.h>

namespace sqloxx
{
namespace tests
{

TEST_FIXTURE(DatabaseConnectionFixture, identity_map_constructors)
{
	// There's not a lot to test here
	IdentityMap<DerivedPO, DatabaseConnection> idm(dbc);

	// Copy constuctor - check it's shallow
	IdentityMap<DerivedPO, DatabaseConnection> idm2(idm);
	CHECK_EQUAL(&(idm2.connection()), &(idm.connection()));

	DatabaseConnection dbc2;

	// Assignment - check it's shallow
	IdentityMap<DerivedPO, DatabaseConnection> idm3(dbc2);
	CHECK(&(idm3.connection()) != &(idm.connection()));
	idm3 = idm2;
	CHECK_EQUAL(&(idm3.connection()), &(idm.connection()));
}

TEST_FIXTURE(DerivedPOFixture, identity_map_provide_handle)
{	
	DerivedDatabaseConnection& dbc = *pdbc;
	IdentityMap<DerivedPO, DerivedDatabaseConnection> idm(dbc);
	Handle<DerivedPO> dpo1(idm.provide_handle());
	dpo1->set_x(100);
	dpo1->set_y(0.5);
	dpo1->save();
	Handle<DerivedPO> dpo2(idm.provide_handle());
	dpo2->set_x(-1098);
	dpo2->set_y(-18);
	dpo2->save();
	Handle<DerivedPO> dpo3(idm.provide_handle(2));
	CHECK_EQUAL(dpo2->id(), dpo3->id());
	CHECK_EQUAL(dpo3->id(), 2);
	CHECK_EQUAL(dpo1->id(), 1);
	Handle<DerivedPO> dpo4(idm.provide_handle(1));
	CHECK_EQUAL(dpo4->id(), 1);
	CHECK_EQUAL(dpo4->x(), 100);
	dpo4->set_x(50);
	CHECK_EQUAL(dpo1->x(), 50);
	dpo1->set_x(-1);
	CHECK_EQUAL(dpo4->x(), -1);
	CHECK_EQUAL(dpo1->x(), -1);
	Handle<DerivedPO> dpo5(idm.provide_handle(1));
	CHECK_EQUAL(dpo5->x(), -1);
}

TEST_FIXTURE(DerivedPOFixture, identity_map_connection)
{
	DerivedDatabaseConnection& dbc = *pdbc;
	IdentityMap<DerivedPO, DerivedDatabaseConnection> idm(dbc);
	CHECK_EQUAL(&(idm.connection()), &dbc);
}


}  // namespace tests
}  // namespace sqloxx
