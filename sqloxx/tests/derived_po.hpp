#include "sqloxx/persistent_object.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

namespace sqloxx
{
namespace tests
{

// Dummy class inheriting from PersistentObject, for the purpose
// of testing PersistentObject class.
class DerivedPO: public PersistentObject
{

public:
	typedef PersistentObject::Id Id;
	static void setup_tables(DatabaseConnection& dbc);
	explicit DerivedPO(boost::shared_ptr<DatabaseConnection> p_dbc);
	DerivedPO(boost::shared_ptr<DatabaseConnection> p_dbc, Id p_id);
	int x();
	double y();
	void set_x(int p_x);
	void set_y(double p_y);
	// Default destructor is OK.
	
private:
	// Copy constructor is deliberately unimplemented.
	DerivedPO(DerivedPO const& rhs);
	void do_load_all();

	// WARNING Need to implement properly
	void do_save_existing_all()
	{
	}

	// WARNING Need to implement properly
	void do_save_existing_partial()
	{
	}

	void do_save_new_all();
	std::string do_get_table_name() const;
	int m_x;
	double m_y;
};


}  // namespace tests
}  // namespace sqloxx

