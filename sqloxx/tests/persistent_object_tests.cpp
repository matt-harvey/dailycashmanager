#include <unittest++/UnitTest++.h>
#include "sqloxx/database_connection.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using boost::shared_ptr;

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

	DerivedPO(shared_ptr<DatabaseConnection> p_dbc, Id p_id):
		PersistentObject(p_dbc, p_id),
		m_x(0),
		m_y(0)
	{
	}

	explicit PersistentObject(shared_ptr<DatabaseConnection> p_dbc):
		PersistentObject(p_dbc):
		m_x(0),
		m_y(0)
	{
	}

	int x() const
	{
		load();
		return m_x;
	}

	double y() const
	{
		load();
		return m_y;
	}

	void set_x(int p_x)
	{
		m_x = p_x;
		return;
	}

	void set_y(double p_y)
	{
		m_y = p_y;
		return;
	}

	// Default destructor is OK.
	
private:

	// Copy constructor is deliberately unimplemented.
	DerivedPO(DerivedPO const& rhs);

	// WARNING Need to define.
	void do_load_all();

	// WARNING Need to define.
	void do_save_existing_all();

	// WARNING Need to define.
	void do_save_existing_partial();

	// WARNING Need to define.
	void do_save_new_all();

	string do_get_table_name() const
	{
		return "derived_pos";
	}

	int m_x;
	double m_y;

};

}  // namespace test
}  // namespace sqloxx

