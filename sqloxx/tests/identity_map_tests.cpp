#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/identity_map.hpp"
#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include <cassert>
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

TEST_FIXTURE(DerivedPOFixture, identity_map_provide_handle_with_caching)
{	
	DerivedDatabaseConnection& dbc = *pdbc;
	IdentityMap<DerivedPO, DerivedDatabaseConnection> idm(dbc);

	idm.disable_caching();  // It's disabled by default, but anyway...
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

TEST_FIXTURE(DerivedPOFixture, identity_map_provide_handle_without_caching)
{
	DerivedDatabaseConnection& dbc = *pdbc;
	IdentityMap<DerivedPO, DerivedDatabaseConnection> idm(dbc);

	// Results should be the same with caching enabled
	idm.enable_caching();
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

TEST_FIXTURE(DerivedPOFixture, identity_map_switch_caching)
{
	// Let's check that the integrity is preserved as we turn
	// caching on at off willy-nilly
	DerivedDatabaseConnection& dbc = *pdbc;
	IdentityMap<DerivedPO, DerivedDatabaseConnection> idm(dbc);

	idm.enable_caching();
	Handle<DerivedPO> dpo1(idm.provide_handle());
	dpo1->set_x(100);
	dpo1->set_y(0.5);
	dpo1->save();
	Handle<DerivedPO> dpo2(idm.provide_handle());
	dpo2->set_x(-1098);
	idm.disable_caching();
	dpo2->set_y(-18);
	dpo2->save();
	idm.enable_caching();
	Handle<DerivedPO> dpo3(idm.provide_handle(2));
	CHECK_EQUAL(dpo2->id(), dpo3->id());
	CHECK_EQUAL(dpo3->id(), 2);
	idm.disable_caching();
	CHECK_EQUAL(dpo1->id(), 1);
	Handle<DerivedPO> dpo4(idm.provide_handle(1));
	CHECK_EQUAL(dpo4->id(), 1);
	CHECK_EQUAL(dpo4->x(), 100);
	dpo4->set_x(50);
	idm.enable_caching();
	idm.disable_caching();
	idm.disable_caching();
	CHECK_EQUAL(dpo1->x(), 50);
	dpo1->set_x(-1);
	CHECK_EQUAL(dpo4->x(), -1);
	idm.enable_caching();
	CHECK_EQUAL(dpo1->x(), -1);
	Handle<DerivedPO> dpo5(idm.provide_handle(1));
	idm.disable_caching();
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
