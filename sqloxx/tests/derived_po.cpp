#include "derived_po.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using boost::shared_ptr;
using std::string;

namespace sqloxx
{
namespace test
{

void
DerivedPO::setup_tables(DatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table derived pos"
		"(derived_po_id integer primary key autoincrement, "
		"x integer not null, y float not null)"
	);
	return;
}

DerivedPO::DerivedPO(shared_ptr<DatabaseConnection> p_dbc, Id p_id);
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
DerivedPO::y()
{
	load();
	return m_y;
}

void
DerivedPO::set_x(int p_x)
{
	m_x = p_x;
	return;
}

void
DerivedPO::set_y(int p_y)
{
	m_y = p_y;
	return;
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
	int temp_y = selector.extract<int>(1);
	selector.step_final();
	m_x = temp_x;
	m_y = temp_y;
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


}  // namespace test
}  // namespace sqloxx
