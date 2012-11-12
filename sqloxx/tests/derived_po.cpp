#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <string>

using boost::shared_ptr;
using std::logic_error;
using std::string;

namespace sqloxx
{
namespace tests
{

void
DerivedPO::setup_tables(DatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table derived_pos"
		"(derived_po_id integer primary key autoincrement, "
		"x integer not null, y float not null)"
	);
	return;
}

DerivedPO::DerivedPO(shared_ptr<DatabaseConnection> p_dbc, Id p_id):
	PersistentObject(p_dbc, p_id),
	m_x(0),
	m_y(0)
{
}

DerivedPO::DerivedPO(shared_ptr<DatabaseConnection> p_dbc):
	PersistentObject(p_dbc),
	m_x(0),
	m_y(0)
{
}

int
DerivedPO::x()
{
	load();
	return m_x;
}

int
DerivedPO::self_test()
{
	int num_failures = 0;
	DerivedPOFixture fixture;
	DerivedPO dpo1(fixture.pdbc);
	dpo1.set_x(3);
	dpo1.set_y(4.08);
	dpo1.save_new();
	if (dpo1.id() != 1) ++num_failures;
	if (dpo1.x() != 3) ++num_failures;
	if (dpo1.y() != 4.08) ++num_failures;
	DerivedPO dpo2(fixture.pdbc, 1);
	if (dpo2.id() != 1) ++num_failures;
	if (dpo2.x() != 3) ++num_failures;
	if (dpo2.y() != 4.08) ++num_failures;

	// Check copy constructor
	DerivedPO dpo3(dpo2);
	if (dpo3.id() != dpo2.id()) ++num_failures;
	if (dpo3.x() != dpo2.x()) ++num_failures;
	if (dpo3.y() != dpo2.y()) ++num_failures;

	// Check swap_base_internals
	DerivedPO dpo4(fixture.pdbc);
	dpo4.set_x(-30);
	dpo4.set_y(-0.01887);
	dpo4.save_new();
	if (dpo4.id() != 2) ++num_failures;
	if (dpo3.id() != 1) ++num_failures;
	dpo4.swap_base_internals(dpo3);
	if (dpo4.id() != 1) ++num_failures;
	if (dpo3.id() != 2) ++num_failures;
	if (dpo4.x() != -30) ++num_failures;
	if (dpo4.y() != -0.01887) ++num_failures;

	// Check database_connection_getter
	if (dpo1.database_connection() != fixture.pdbc) ++num_failures;
	if (dpo4.database_connection() != fixture.pdbc) ++num_failures;

	// Check prospective_key() && do_calculate_prospective_key() (default)
	DerivedPO dpo5(fixture.pdbc);
	if (dpo5.prospective_key() != 3) ++num_failures;
	dpo5.set_x(-100);
	dpo5.set_y(982734);
	if (dpo5.prospective_key() != 3) ++num_failures;
	if (dpo5.do_calculate_prospective_key() != 3) ++num_failures;
	bool ok = false;
	try
	{
		dpo1.prospective_key();
	}
	catch (logic_error&)
	{
		ok = true;
	}
	if (!ok) ++num_failures;	

	// Check set_id()
	DerivedPO dpo6(fixture.pdbc);
	dpo6.save_new();
	if (dpo6.id() != 3) ++num_failures;
	ok = false;
	try
	{
		dpo6.set_id(10);
	}
	catch (logic_error&)
	{
		ok = true;
	}
	if (ok != true) ++num_failures;
	DerivedPO dpo6b(fixture.pdbc);
	dpo6b.set_id(12);
	if (dpo6b.id() != 12) ++num_failures;
	
	// Check has_id()
	if (!dpo1.has_id()) ++num_failures;
	DerivedPO dpo7(fixture.pdbc);
	if (dpo7.has_id()) ++num_failures;

	return num_failures;
}

double
DerivedPO::y()
{
	load();
	return m_y;
}

void
DerivedPO::set_x(int p_x)
{
	load();
	m_x = p_x;
	return;
}

void
DerivedPO::set_y(double p_y)
{
	load();
	m_y = p_y;
	return;
}

DerivedPO::DerivedPO(DerivedPO const& rhs):
	PersistentObject(rhs),
	m_x(rhs.m_x),
	m_y(rhs.m_y)
{
}

void
DerivedPO::do_load_all()
{
	SharedSQLStatement selector
	(	*database_connection(),
		"select x, y from derived_pos where derived_po_id = :p"
	);
	selector.bind(":p", id());
	selector.step();
	int temp_x = selector.extract<int>(0);
	double temp_y = selector.extract<double>(1);
	selector.step_final();
	set_x(temp_x);
	set_y(temp_y);
}

void
DerivedPO::do_save_existing_all()
{
	SharedSQLStatement updater
	(	*database_connection(),
		"update derived_pos set x = :x, y = :y where derived_po_id = :id"
	);
	updater.bind(":x", m_x);
	updater.bind(":y", m_y);
	updater.bind(":id", id());
	updater.step_final();
	return;
}

void
DerivedPO::do_save_new_all()
{
	SharedSQLStatement inserter
	(	*database_connection(),
		"insert into derived_pos(x, y) values(:x, :y)"
	);
	inserter.bind(":x", m_x);
	inserter.bind(":y", m_y);
	inserter.step_final();
}

string
DerivedPO::do_get_table_name() const
{
	return "derived_pos";
}


}  // namespace tests
}  // namespace sqloxx
