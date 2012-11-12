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
	
	// To test protected functions of PersistentObject. Returns number
	// of failing checks in test.
	static int self_test();	

protected:
	DerivedPO(DerivedPO const& rhs);

private:
	void do_load_all();
	// Uses default version of do_calculate_prospective_key
	void do_save_existing_all();
	void do_save_new_all();
	std::string do_get_table_name() const;
	int m_x;
	double m_y;
};


}  // namespace tests
}  // namespace sqloxx

