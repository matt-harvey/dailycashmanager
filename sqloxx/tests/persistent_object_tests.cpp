#include <unittest++/UnitTest++.h>
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

// Dummy class inheriting from PersistentObject, for the purpose
// of testing PersistentObject class.
class DerivedPO: public PersistentObject
{
public:

	typedef PersistentObject::Id Id;

	void setup_tables(DatabaseConnection& dbc)
	{
		dbc.execute_sql
		(	"create table derived pos"
			"(derived_po_id integer primary key autoincrement, "
			"x integer not null, y float not null)"
		);
		return;
	}

	DerivedPO(shared_ptr<DatabaseConnection> p_dbc, Id p_id):
		PersistentObject(p_dbc, p_id),
		m_x(0),
		m_y(0)
	{
	}

	explicit DerivedPO(shared_ptr<DatabaseConnection> p_dbc):
		PersistentObject(p_dbc),
		m_x(0),
		m_y(0)
	{
	}

	int x()
	{
		load();
		return m_x;
	}

	int y()
	{
		load();
		return m_y;
	}

	void set_x(int p_x)
	{
		m_x = p_x;
		return;
	}

	void set_y(int p_y)
	{
		m_y = p_y;
		return;
	}

	// Default destructor is OK.
	
private:

	// Copy constructor is deliberately unimplemented.
	DerivedPO(DerivedPO const& rhs);

	void do_load_all()
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

	// WARNING Need to implement properly
	void do_save_existing_all()
	{
	}

	// WARNING Need to implement properly
	void do_save_existing_partial()
	{
	}

	void do_save_new_all()
	{
		SharedSQLStatement inserter
		(	*database_connection(),
			"insert into derived_pos(x, y) values(:x, :y)"
		);
		inserter.bind(":x", m_x);
		inserter.bind(":y", m_y);
		inserter.step_final();
	}

	string do_get_table_name() const
	{
		return "derived_pos";
	}

	int m_x;
	int m_y;

};


}  // namespace test
}  // namespace sqloxx

