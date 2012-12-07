#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include "sqloxx/sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using boost::shared_ptr;
using std::string;

namespace sqloxx
{
namespace tests
{

void
DerivedPO::setup_tables(DerivedDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table derived_pos"
		"(derived_po_id integer primary key autoincrement, "
		"x integer not null, y float not null)"
	);
	return;
}

DerivedPO::DerivedPO
(	IdentityMap& p_identity_map,
	Id p_id
):
	PersistentObject
	(	p_identity_map,
		p_id
	),
	m_x(0),
	m_y(0)
{
}

DerivedPO::DerivedPO
(	IdentityMap& p_identity_map
):
	PersistentObject(p_identity_map),
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
	DerivedDatabaseConnection& dbc = *fixture.pdbc;
	Handle<DerivedPO> dpo1(get_handle<DerivedPO>(dbc));
	dpo1->set_x(3);
	dpo1->set_y(4.08);
	dpo1->save();
	if (dpo1->id() != 1) ++num_failures;
	if (dpo1->x() != 3) ++num_failures;
	if (dpo1->y() != 4.08) ++num_failures;
	Handle<DerivedPO> dpo2(get_handle<DerivedPO>(dbc, 1));
	if (dpo2->id() != 1) ++num_failures;
	if (dpo2->x() != 3) ++num_failures;
	if (dpo2->y() != 4.08) ++num_failures;

	// Check prospective_key() && do_calculate_prospective_key() (default)
	Handle<DerivedPO> dpo5(get_handle<DerivedPO>(dbc));
	if (dpo5->prospective_key() != 2) ++num_failures;
	dpo5->set_x(-100);
	dpo5->set_y(982734);
	if (dpo5->prospective_key() != 2) ++num_failures;
	bool ok = false;
	try
	{
		dpo1->prospective_key();
	}
	catch (LogicError&)
	{
		ok = true;
	}
	if (!ok) ++num_failures;	
	
	// Check has_id()
	if (!dpo1->has_id()) ++num_failures;
	Handle<DerivedPO> dpo7(get_handle<DerivedPO>(dbc));
	if (dpo7->has_id()) ++num_failures;

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
DerivedPO::do_load()
{
	SQLStatement selector
	(	database_connection(),
		"select x, y from derived_pos where derived_po_id = :p"
	);
	selector.bind(":p", id());
	selector.step();
	int temp_x = selector.extract<int>(0);
	double temp_y = selector.extract<double>(1);
	selector.step_final();
	m_x = temp_x;
	m_y = temp_y;
}

void
DerivedPO::do_save_existing()
{
	SQLStatement updater
	(	database_connection(),
		"update derived_pos set x = :x, y = :y where derived_po_id = :id"
	);
	updater.bind(":x", m_x);
	updater.bind(":y", m_y);
	updater.bind(":id", id());
	updater.step_final();
	return;
}

void
DerivedPO::do_save_new()
{
	SQLStatement inserter
	(	database_connection(),
		"insert into derived_pos(x, y) values(:x, :y)"
	);
	inserter.bind(":x", m_x);
	inserter.bind(":y", m_y);
	inserter.step_final();
}

void
DerivedPO::do_ghostify()
{
	// No point doing anything here really, but in any
	// case let's set m_x and m_y to values that look
	// suspicious, to suggest ghostness.
	m_x = -999999999;
	m_y = -9.99999999;
}

string
DerivedPO::primary_table_name()
{
	return "derived_pos";
}

string
DerivedPO::primary_key_name()
{
	return "derived_po_id";
}

DerivedDatabaseConnection::DerivedDatabaseConnection():
	DatabaseConnection(),
	m_derived_po_map(*this)
{
}

}  // namespace tests
}  // namespace sqloxx
